#pragma once

#include <QLatin1StringView>
#include <QString>
#include <QtGlobal>

// Every aa-usb log text. The marker list is docs/traces/README.md, row Z1.
namespace AoaMarkers {

inline constexpr QLatin1StringView COMPONENT{"aa-usb"};
inline constexpr QLatin1StringView WATCHING{"watching for phones"};
inline constexpr QLatin1StringView WATCH_FAILED{"usb watch failed: %1"};
inline constexpr QLatin1StringView LEFTOVER{"accessory left over from an earlier run, resetting it"};
inline constexpr QLatin1StringView RESET_FAILED{"reset: %1"};
inline constexpr QLatin1StringView CANDIDATE{"phone candidate %1, switching to accessory mode"};
inline constexpr QLatin1StringView REQUESTED{"accessory mode requested"};
inline constexpr QLatin1StringView ACCESSORY_UP{"accessory up in %1 ms"};
inline constexpr QLatin1StringView AOAP_FAILED{"aoap: %1"};
inline constexpr QLatin1StringView OPEN_FAILED{"open accessory: %1"};
inline constexpr QLatin1StringView NO_ACCESSORY{"no accessory within %1 ms"};
inline constexpr QLatin1StringView CLOSED{"closed (phone unplugged)"};

inline QString deviceLabel(const QString &serial)
{
    return serial.isEmpty() ? QStringLiteral("usb:phone") : QStringLiteral("usb:") + serial;
}

inline QString vidPid(quint16 vendorId, quint16 productId)
{
    return QStringLiteral("%1:%2")
        .arg(vendorId, 4, 16, QLatin1Char('0'))
        .arg(productId, 4, 16, QLatin1Char('0'));
}

// aa-usb: <label>: <text>, or aa-usb: <text> when label is empty
inline QString line(const QString &label, const QString &text)
{
    if (label.isEmpty()) {
        return QStringLiteral("%1: %2").arg(COMPONENT, text);
    }
    return QStringLiteral("%1: %2: %3").arg(COMPONENT, label, text);
}

}
