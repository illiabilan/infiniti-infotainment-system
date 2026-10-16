#include "SocketCanBackend.h"
#include <QSocketNotifier>
#include <QDebug>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

SocketCanBackend::SocketCanBackend(QObject *parent)
    : CanInterface(parent)
{
}

SocketCanBackend::SocketCanBackend(const QString &interfaceName, QObject *parent)
    : CanInterface(parent)
    , m_interfaceName(interfaceName)
{
}

SocketCanBackend::~SocketCanBackend()
{
    close();
}

bool SocketCanBackend::open(const QString &interfaceName)
{
#ifdef __linux__
    close();

    if (!interfaceName.isEmpty()) {
        m_interfaceName = interfaceName;
    } else if (m_interfaceName.isEmpty()) {
        m_interfaceName = QStringLiteral("vcan0");
    }
    m_socketFd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socketFd < 0) {
        emit errorOccurred(QString("Failed to create CAN socket: %1").arg(std::strerror(errno)));
        return false;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, m_interfaceName.toLatin1().constData(), IFNAMSIZ - 1);

    if (::ioctl(m_socketFd, SIOCGIFINDEX, &ifr) < 0) {
        emit errorOccurred(QString("Failed to find CAN interface %1: %2").arg(m_interfaceName, std::strerror(errno)));
        ::close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(m_socketFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        emit errorOccurred(QString("Failed to bind CAN socket to %1: %2").arg(m_interfaceName, std::strerror(errno)));
        ::close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    m_notifier = new QSocketNotifier(m_socketFd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &SocketCanBackend::onSocketActivated);

    qInfo() << "SocketCAN opened on" << m_interfaceName;
    return true;
#else
    m_interfaceName = interfaceName;
    emit errorOccurred(QString("SocketCAN is not supported on this platform (%1)").arg(interfaceName));
    return false;
#endif
}

void SocketCanBackend::close()
{
#ifdef __linux__
    if (m_notifier) {
        m_notifier->setEnabled(false);
        delete m_notifier;
        m_notifier = nullptr;
    }
    if (m_socketFd >= 0) {
        ::close(m_socketFd);
        m_socketFd = -1;
    }
#endif
}

bool SocketCanBackend::isOpen() const
{
    return m_socketFd >= 0;
}

bool SocketCanBackend::sendFrame(const CanFrame &frame)
{
#ifdef __linux__
    if (m_socketFd < 0) {
        emit errorOccurred("CAN socket is not open");
        return false;
    }

    struct can_frame cf;
    std::memset(&cf, 0, sizeof(cf));
    cf.can_id = frame.canId;
    cf.can_dlc = frame.dlc;
    std::memcpy(cf.data, frame.data, frame.dlc);

    ssize_t bytesWritten = ::write(m_socketFd, &cf, sizeof(cf));
    if (bytesWritten != sizeof(cf)) {
        emit errorOccurred(QString("Failed to write frame to CAN socket: %1").arg(std::strerror(errno)));
        return false;
    }
    return true;
#else
    Q_UNUSED(frame);
    return false;
#endif
}

void SocketCanBackend::onSocketActivated()
{
#ifdef __linux__
    struct can_frame cf;
    ssize_t bytesRead = ::read(m_socketFd, &cf, sizeof(cf));
    if (bytesRead < 0) {
        emit errorOccurred(QString("Error reading CAN socket: %1").arg(std::strerror(errno)));
        return;
    }

    if (bytesRead == sizeof(cf)) {
        CanFrame f = CanFrame::make(cf.can_id, cf.can_dlc, cf.data);
        emit frameReceived(f);
    }
#endif
}
