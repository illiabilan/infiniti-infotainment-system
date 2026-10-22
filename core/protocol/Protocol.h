#pragma once

#include <QJsonObject>
#include <QLatin1StringView>
#include <QString>
#include <QStringList>

// Core <-> client wire protocol v1 (docs/design/clients.md section 4).
namespace Protocol {

inline constexpr int VERSION = 1;

namespace MessageType {
inline constexpr QLatin1StringView HELLO{"hello"};
inline constexpr QLatin1StringView SNAPSHOT{"snapshot"};
inline constexpr QLatin1StringView VEHICLE{"vehicle"};
inline constexpr QLatin1StringView VEHICLE_EVENT{"vehicle_event"};
inline constexpr QLatin1StringView VEHICLE_SET{"vehicle_set"};
// Not named ERROR because of the Windows ERROR macro
inline constexpr QLatin1StringView ERROR_MESSAGE{"error"};
}

namespace ErrorCode {
inline constexpr QLatin1StringView INVALID_JSON{"invalid_json"};
inline constexpr QLatin1StringView UNSUPPORTED_VERSION{"unsupported_version"};
inline constexpr QLatin1StringView INVALID_MESSAGE{"invalid_message"};
inline constexpr QLatin1StringView UNKNOWN_TYPE{"unknown_type"};
inline constexpr QLatin1StringView NOT_GREETED{"not_greeted"};
inline constexpr QLatin1StringView INVALID_HELLO{"invalid_hello"};
inline constexpr QLatin1StringView FORBIDDEN_GROUP{"forbidden_group"};
}

namespace Role {
inline constexpr QLatin1StringView DASH{"dash"};
inline constexpr QLatin1StringView CLUSTER{"cluster"};
inline constexpr QLatin1StringView CLIMATE{"climate"};
}

namespace Want {
inline constexpr QLatin1StringView VEHICLE{"vehicle"};
inline constexpr QLatin1StringView VIDEO{"video"};
}

// Control characters are rejected in client_id and stripped from client text before logging
inline bool isControlChar(QChar c)
{
    return c.unicode() < 0x20 || c.unicode() == 0x7F;
}

inline QString withoutControlChars(QString text)
{
    text.removeIf(isControlChar);
    return text;
}

struct HelloInfo {
    QString clientId;
    QString role;
    QString displayId;
    int width = 0;
    int height = 0;
    QStringList wants;
};

struct ParseResult {
    bool ok = false;
    QJsonObject message;
    QString errorCode;
    QString errorText;
};

// Validates JSON, version and type, in that order
ParseResult parse(const QString &text);

QString typeOf(const QJsonObject &message);

// Applies the hello rules. Returns false with a reason in errorText.
bool parseHello(const QJsonObject &message, HelloInfo &out, QString &errorText);

QJsonObject makeHello(const HelloInfo &hello);
QJsonObject makeSnapshot(const QJsonObject &session, const QJsonObject &vehicle);
QJsonObject makeVehicle(const QJsonObject &values);
QJsonObject makeVehicleEvent(const QString &name);
QJsonObject makeVehicleSet(const QString &group, int action, const QJsonObject &values);
QJsonObject makeError(const QString &code, const QString &text);

// Compact JSON text
QString toText(const QJsonObject &message);

} // namespace Protocol
