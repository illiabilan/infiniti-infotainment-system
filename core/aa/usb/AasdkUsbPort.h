#pragma once

#include "AoaUsbPort.h"
#include <memory>

// AoaUsbPort over aasdk src/USB/ and libusb. Built only with AA_WITH_AASDK=ON.
// It does not use aasdk USBHub, which sleeps 1000 ms per non-accessory device.
// The PIMPL keeps Boost, aasdk and libusb headers out of every includer.
class AasdkUsbPort final : public AoaUsbPort
{
    Q_OBJECT

public:
    explicit AasdkUsbPort(QObject *parent = nullptr);
    ~AasdkUsbPort() override;

    bool start(QString *errorText) override;
    void requestAccessoryMode(quint64 id) override;
    bool openAccessory(quint64 id, QString *errorText) override;
    bool resetDevice(quint64 id, QString *errorText) override;

private:
    class Backend;
    std::unique_ptr<Backend> m_backend;
};
