#pragma once

#include "UsbDeviceInfo.h"
#include <QObject>
#include <QString>

// The I/O seam between the Z1 state machine and USB.
// Contract:
// - deviceArrived fires once per attached device, unfiltered. atStartup is true for devices
//   already attached when start() ran.
// - deviceLeft fires only for ids previously reported.
// - requestAccessoryMode is asynchronous and ends in exactly one of accessoryModeRequested
//   or accessoryModeFailed.
// - openAccessory and resetDevice are synchronous. On false, *errorText holds a short reason.
// - Signals are never emitted from inside a call into the port.
class AoaUsbPort : public QObject
{
    Q_OBJECT

public:
    explicit AoaUsbPort(QObject *parent = nullptr) : QObject(parent) {}
    ~AoaUsbPort() override = default;

    virtual bool start(QString *errorText) = 0;
    virtual void requestAccessoryMode(quint64 id) = 0;
    virtual bool openAccessory(quint64 id, QString *errorText) = 0;
    virtual bool resetDevice(quint64 id, QString *errorText) = 0;

signals:
    void deviceArrived(const UsbDeviceInfo &info, bool atStartup);
    void deviceLeft(quint64 id);
    void accessoryModeRequested(quint64 id);
    void accessoryModeFailed(quint64 id, const QString &errorText);
};
