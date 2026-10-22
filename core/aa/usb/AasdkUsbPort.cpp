#include "AasdkUsbPort.h"

#include "AoaDeviceFilter.h"
#include <QMetaObject>
#include <aasdk/Error/Error.hpp>
#include <aasdk/USB/AOAPDevice.hpp>
#include <aasdk/USB/AccessoryModeQueryChainFactory.hpp>
#include <aasdk/USB/AccessoryModeQueryFactory.hpp>
#include <aasdk/USB/IAccessoryModeQueryChain.hpp>
#include <aasdk/USB/USBWrapper.hpp>
#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <libusb.h>
#include <map>
#include <optional>
#include <thread>

namespace {

constexpr int EVENT_POLL_MS = 100;
constexpr int SHUTDOWN_DRAIN_MS = 2000;
constexpr int DRAIN_POLL_MS = 5;
constexpr int SERIAL_BUFFER_SIZE = 128;
constexpr char PRINTABLE_FIRST = 0x21;
constexpr char PRINTABLE_LAST = 0x7e;

quint64 idOf(libusb_device *device)
{
    return (quint64(libusb_get_bus_number(device)) << 8) | libusb_get_device_address(device);
}

QString usbError(const char *prefix, int code)
{
    return QStringLiteral("%1: %2").arg(QLatin1String(prefix), QLatin1String(libusb_error_name(code)));
}

}

// Threading: the libusb event thread only pumps events, the io thread only runs aasdk chain I/O.
// The device maps and the synchronous libusb calls live on the Qt thread, so there is no mutex.
class AasdkUsbPort::Backend
{
public:
    explicit Backend(AasdkUsbPort *port) : m_port(port) {}
    ~Backend();

    bool start(QString *errorText);
    void requestAccessoryMode(quint64 id);
    bool openAccessory(quint64 id, QString *errorText);
    bool resetDevice(quint64 id, QString *errorText);

    // Runs on the libusb thread, or on the Qt thread during the startup enumeration.
    // It only posts work to the Qt thread, as libusb.h recommends.
    static int LIBUSB_CALL onHotplug(libusb_context *, libusb_device *device, libusb_hotplug_event event, void *userData);

private:
    using IoWork = boost::asio::executor_work_guard<boost::asio::io_service::executor_type>;

    // Chain handlers run on the io thread and can fire while the destructor cancels the chains, so they skip the Qt thread then
    template<typename Functor>
    void post(Functor &&functor)
    {
        if (m_closing) {
            return;
        }
        QMetaObject::invokeMethod(m_port, std::forward<Functor>(functor), Qt::QueuedConnection);
    }

    void hotplugArrived(libusb_device *device);
    void hotplugLeft(libusb_device *device);
    void handleArrived(libusb_device *device, bool atStartup);
    void handleLeft(quint64 id);
    UsbDeviceInfo describe(libusb_device *device, quint64 id) const;
    QString readSerial(libusb_device *device, quint8 index) const;
    void runEventLoop();

    AasdkUsbPort *m_port;
    libusb_context *m_context{nullptr};
    libusb_hotplug_callback_handle m_hotplugHandle{};
    bool m_hotplugRegistered{false};
    // Written on the Qt thread before the event thread starts
    bool m_enumerating{false};
    std::atomic<bool> m_stopping{false};
    std::atomic<bool> m_closing{false};
    // Counts chains whose promise handler has not run yet, the destructor waits for it after cancel()
    std::atomic<int> m_activeChains{0};
    // Heap-allocated so the destructor can destroy leftover handlers (they own libusb handles) before libusb_exit
    std::unique_ptr<boost::asio::io_service> m_ioService{std::make_unique<boost::asio::io_service>()};
    std::optional<IoWork> m_work;
    std::unique_ptr<aasdk::usb::USBWrapper> m_usbWrapper;
    std::unique_ptr<aasdk::usb::AccessoryModeQueryFactory> m_queryFactory;
    std::unique_ptr<aasdk::usb::AccessoryModeQueryChainFactory> m_chainFactory;
    // Each entry holds one libusb_ref_device reference
    std::map<quint64, libusb_device *> m_devices;
    std::map<quint64, aasdk::usb::IAccessoryModeQueryChain::Pointer> m_chains;
    std::map<quint64, aasdk::usb::IAOAPDevice::Pointer> m_accessories;
    std::thread m_usbEvents;
    std::thread m_ioThread;
};

// Shutdown order, every step needs the one before it:
// 1. deregister the hotplug callback and set m_closing, so no new work reaches the Qt thread
// 2. release the accessories and the device references (Qt thread)
// 3. cancel() every chain and wait until their promise handlers ran, the libusb events thread and the io thread keep
//    running so the cancelled transfers complete (bounded by SHUTDOWN_DRAIN_MS)
// 4. drop the work guard and join the io thread, which drains all queued handlers (they own libusb handles)
// 5. join the libusb events thread
// 6. destroy the io_service, which destroys any handler left after a drain timeout while the context is still valid
// 7. reset the aasdk objects and call libusb_exit
AasdkUsbPort::Backend::~Backend()
{
    if (m_hotplugRegistered) {
        libusb_hotplug_deregister_callback(m_context, m_hotplugHandle);
    }
    m_closing = true;

    m_accessories.clear();
    for (auto &entry : m_devices) {
        libusb_unref_device(entry.second);
    }
    m_devices.clear();

    for (auto &entry : m_chains) {
        entry.second->cancel();
    }
    const auto drainDeadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(SHUTDOWN_DRAIN_MS);
    while (m_activeChains > 0 && std::chrono::steady_clock::now() < drainDeadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(DRAIN_POLL_MS));
    }
    m_chains.clear();

    m_work.reset();
    if (m_ioThread.joinable()) {
        m_ioThread.join();
    }

    m_stopping = true;
    if (m_usbEvents.joinable()) {
        m_usbEvents.join();
    }

    m_ioService.reset();
    m_chainFactory.reset();
    m_queryFactory.reset();
    m_usbWrapper.reset();

    if (m_context != nullptr) {
        libusb_exit(m_context);
    }
}

bool AasdkUsbPort::Backend::start(QString *errorText)
{
    if (m_context != nullptr) {
        *errorText = QStringLiteral("already started");
        return false;
    }

    const int initResult = libusb_init(&m_context);
    if (initResult != 0) {
        m_context = nullptr;
        *errorText = usbError("libusb init", initResult);
        return false;
    }
    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        *errorText = QStringLiteral("libusb has no hotplug support");
        return false;
    }

    m_usbWrapper = std::make_unique<aasdk::usb::USBWrapper>(m_context);
    m_queryFactory = std::make_unique<aasdk::usb::AccessoryModeQueryFactory>(*m_usbWrapper, *m_ioService);
    m_chainFactory = std::make_unique<aasdk::usb::AccessoryModeQueryChainFactory>(
        *m_usbWrapper, *m_ioService, *m_queryFactory);

    m_work.emplace(boost::asio::make_work_guard(*m_ioService));
    m_ioThread = std::thread([this] { m_ioService->run(); });

    // aasdk USBWrapper::hotplugRegisterCallback ignores the result and keeps a stack address, so libusb is called directly.
    // LIBUSB_HOTPLUG_ENUMERATE calls the callback from inside the register call, which marks startup devices.
    m_enumerating = true;
    const int hotplugResult = libusb_hotplug_register_callback(
        m_context,
        static_cast<libusb_hotplug_event>(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
        LIBUSB_HOTPLUG_ENUMERATE, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
        &Backend::onHotplug, this, &m_hotplugHandle);
    m_enumerating = false;
    if (hotplugResult != 0) {
        *errorText = usbError("hotplug register", hotplugResult);
        return false;
    }
    m_hotplugRegistered = true;

    m_usbEvents = std::thread([this] { runEventLoop(); });
    return true;
}

void AasdkUsbPort::Backend::runEventLoop()
{
    timeval timeout{0, EVENT_POLL_MS * 1000};
    while (!m_stopping) {
        libusb_handle_events_timeout_completed(m_context, &timeout, nullptr);
    }
}

int LIBUSB_CALL AasdkUsbPort::Backend::onHotplug(libusb_context *, libusb_device *device,
                                                  libusb_hotplug_event event, void *userData)
{
    auto *backend = static_cast<Backend *>(userData);
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        backend->hotplugArrived(device);
    } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        backend->hotplugLeft(device);
    }
    return 0;
}

void AasdkUsbPort::Backend::hotplugArrived(libusb_device *device)
{
    libusb_ref_device(device);
    const bool atStartup = m_enumerating;
    post([this, device, atStartup] { handleArrived(device, atStartup); });
}

void AasdkUsbPort::Backend::hotplugLeft(libusb_device *device)
{
    const quint64 id = idOf(device);
    post([this, id] { handleLeft(id); });
}

void AasdkUsbPort::Backend::handleArrived(libusb_device *device, bool atStartup)
{
    const quint64 id = idOf(device);
    // Drops the duplicate arrival that LIBUSB_HOTPLUG_ENUMERATE can produce
    if (m_devices.contains(id)) {
        libusb_unref_device(device);
        return;
    }
    m_devices.emplace(id, device);
    emit m_port->deviceArrived(describe(device, id), atStartup);
}

void AasdkUsbPort::Backend::handleLeft(quint64 id)
{
    const auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        return;
    }
    m_chains.erase(id);
    m_accessories.erase(id);
    libusb_unref_device(it->second);
    m_devices.erase(it);
    emit m_port->deviceLeft(id);
}

UsbDeviceInfo AasdkUsbPort::Backend::describe(libusb_device *device, quint64 id) const
{
    UsbDeviceInfo info;
    info.id = id;

    libusb_device_descriptor descriptor{};
    if (m_usbWrapper->getDeviceDescriptor(device, descriptor) != 0) {
        return info;
    }
    info.vendorId = descriptor.idVendor;
    info.productId = descriptor.idProduct;
    info.deviceClass = descriptor.bDeviceClass;

    aasdk::usb::ConfigDescriptorHandle config;
    if (m_usbWrapper->getConfigDescriptor(device, 0, config) == 0 && config != nullptr) {
        for (int i = 0; i < config->bNumInterfaces; ++i) {
            const libusb_interface &interface = config->interface[i];
            if (interface.num_altsetting > 0) {
                info.interfaceClasses.append(interface.altsetting[0].bInterfaceClass);
            }
        }
    }

    const bool relevant = AoaDeviceFilter::isAccessory(info) || AoaDeviceFilter::isCandidate(info);
    if (relevant && descriptor.iSerialNumber != 0) {
        info.serial = readSerial(device, descriptor.iSerialNumber);
    }
    return info;
}

QString AasdkUsbPort::Backend::readSerial(libusb_device *device, quint8 index) const
{
    aasdk::usb::DeviceHandle handle;
    if (m_usbWrapper->open(device, handle) != 0) {
        return {};
    }

    unsigned char buffer[SERIAL_BUFFER_SIZE] = {};
    const int length = libusb_get_string_descriptor_ascii(handle.get(), index, buffer, sizeof(buffer));
    if (length <= 0) {
        return {};
    }

    // Device text is sanitized before logging
    QString serial;
    for (int i = 0; i < length; ++i) {
        const char byte = static_cast<char>(buffer[i]);
        if (byte >= PRINTABLE_FIRST && byte <= PRINTABLE_LAST) {
            serial.append(QLatin1Char(byte));
        }
    }
    return serial;
}

void AasdkUsbPort::Backend::requestAccessoryMode(quint64 id)
{
    const auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        post([this, id] { emit m_port->accessoryModeFailed(id, QStringLiteral("device is gone")); });
        return;
    }

    aasdk::usb::DeviceHandle handle;
    const int openResult = m_usbWrapper->open(it->second, handle);
    if (openResult != 0) {
        const QString text = usbError("open", openResult);
        post([this, id, text] { emit m_port->accessoryModeFailed(id, text); });
        return;
    }

    auto chain = m_chainFactory->create();
    m_chains[id] = chain;

    auto promise = aasdk::usb::IAccessoryModeQueryChain::Promise::defer(*m_ioService);
    promise->then(
        [this, id](aasdk::usb::DeviceHandle) {
            post([this, id] {
                m_chains.erase(id);
                emit m_port->accessoryModeRequested(id);
            });
            --m_activeChains;
        },
        [this, id](const aasdk::error::Error &error) {
            const QString text = QString::fromUtf8(error.what());
            post([this, id, text] {
                m_chains.erase(id);
                emit m_port->accessoryModeFailed(id, text);
            });
            --m_activeChains;
        });

    // cancel() is only called by the destructor, a timed-out request is dropped by the switcher through its id and state checks
    ++m_activeChains;
    chain->start(std::move(handle), std::move(promise));
}

bool AasdkUsbPort::Backend::openAccessory(quint64 id, QString *errorText)
{
    const auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        *errorText = QStringLiteral("device is gone");
        return false;
    }

    aasdk::usb::DeviceHandle handle;
    const int openResult = m_usbWrapper->open(it->second, handle);
    if (openResult != 0) {
        *errorText = QLatin1String(libusb_error_name(openResult));
        return false;
    }

    try {
        // The accessory stays held, interface claimed, until the device leaves or shutdown
        // Known aasdk use-after-free in ~AOAPDevice, see "Відомий дефект aasdk: AOAPDevice" in docs/tickets/Z1-aoa-contract.md
        m_accessories[id] = aasdk::usb::AOAPDevice::create(*m_usbWrapper, *m_ioService, std::move(handle));
    } catch (const aasdk::error::Error &error) {
        *errorText = QString::fromUtf8(error.what());
        return false;
    }
    return true;
}

bool AasdkUsbPort::Backend::resetDevice(quint64 id, QString *errorText)
{
    const auto it = m_devices.find(id);
    if (it == m_devices.end()) {
        *errorText = QStringLiteral("device is gone");
        return false;
    }

    aasdk::usb::DeviceHandle handle;
    const int openResult = m_usbWrapper->open(it->second, handle);
    if (openResult != 0) {
        *errorText = usbError("open", openResult);
        return false;
    }

    // NOT_FOUND means the device re-enumerated, which is the goal of the reset
    const int resetResult = libusb_reset_device(handle.get());
    if (resetResult != 0 && resetResult != LIBUSB_ERROR_NOT_FOUND) {
        *errorText = QLatin1String(libusb_error_name(resetResult));
        return false;
    }
    return true;
}

AasdkUsbPort::AasdkUsbPort(QObject *parent)
    : AoaUsbPort(parent)
    , m_backend(std::make_unique<Backend>(this))
{
}

AasdkUsbPort::~AasdkUsbPort() = default;

bool AasdkUsbPort::start(QString *errorText)
{
    return m_backend->start(errorText);
}

void AasdkUsbPort::requestAccessoryMode(quint64 id)
{
    m_backend->requestAccessoryMode(id);
}

bool AasdkUsbPort::openAccessory(quint64 id, QString *errorText)
{
    return m_backend->openAccessory(id, errorText);
}

bool AasdkUsbPort::resetDevice(quint64 id, QString *errorText)
{
    return m_backend->resetDevice(id, errorText);
}
