#pragma once

#include <QList>
#include <QString>
#include <QtGlobal>

// Descriptor facts of one attached USB device, as reported by the port.
struct UsbDeviceInfo {
    // Port-assigned key, unique while the device is attached
    quint64 id = 0;
    quint16 vendorId = 0;
    quint16 productId = 0;
    // bDeviceClass
    quint8 deviceClass = 0;
    // bInterfaceClass of altsetting 0 of every interface in configuration 0. Empty when unreadable.
    QList<quint8> interfaceClasses;
    // Printable ASCII 0x21-0x7E only. Empty when absent or unreadable.
    QString serial;
};
