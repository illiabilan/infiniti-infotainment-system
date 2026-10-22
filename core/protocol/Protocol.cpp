#include "Protocol.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>

namespace Protocol {

static constexpr qsizetype MAX_CLIENT_ID_LENGTH = 64;

static QJsonObject envelope(QLatin1StringView type)
{
    QJsonObject message;
    message.insert(QStringLiteral("type"), QString(type));
    message.insert(QStringLiteral("v"), VERSION);
    return message;
}

ParseResult parse(const QString &text)
{
    ParseResult result;

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(text.toUtf8(), &error);
    if (document.isNull() || !document.isObject()) {
        result.errorCode = QString(ErrorCode::INVALID_JSON);
        result.errorText = error.error != QJsonParseError::NoError ? error.errorString()
                                                                   : QStringLiteral("expected a JSON object");
        return result;
    }

    const QJsonObject message = document.object();
    const QJsonValue version = message.value(QStringLiteral("v"));
    if (!version.isDouble() || version.toInt(-1) != VERSION || version.toDouble() != double(VERSION)) {
        result.errorCode = QString(ErrorCode::UNSUPPORTED_VERSION);
        result.errorText = QStringLiteral("expected v=1");
        return result;
    }

    const QJsonValue type = message.value(QStringLiteral("type"));
    if (!type.isString() || type.toString().isEmpty()) {
        result.errorCode = QString(ErrorCode::INVALID_MESSAGE);
        result.errorText = QStringLiteral("type must be a non-empty string");
        return result;
    }

    result.ok = true;
    result.message = message;
    return result;
}

QString typeOf(const QJsonObject &message)
{
    return message.value(QStringLiteral("type")).toString();
}

bool parseHello(const QJsonObject &message, HelloInfo &out, QString &errorText)
{
    const QString clientId = message.value(QStringLiteral("client_id")).toString();
    if (clientId.isEmpty()) {
        errorText = QStringLiteral("client_id must be a non-empty string");
        return false;
    }
    if (clientId.size() > MAX_CLIENT_ID_LENGTH || std::any_of(clientId.cbegin(), clientId.cend(), isControlChar)) {
        errorText = QStringLiteral("client_id must be at most 64 characters without control characters");
        return false;
    }

    const QString role = message.value(QStringLiteral("role")).toString();
    if (role != Role::DASH && role != Role::CLUSTER && role != Role::CLIMATE) {
        errorText = QStringLiteral("role must be dash, cluster or climate");
        return false;
    }

    HelloInfo info;
    info.clientId = clientId;
    info.role = role;

    const QJsonObject display = message.value(QStringLiteral("display")).toObject();
    info.displayId = display.value(QStringLiteral("id")).toString();
    info.width = display.value(QStringLiteral("w")).toInt();
    info.height = display.value(QStringLiteral("h")).toInt();

    const QJsonArray wants = message.value(QStringLiteral("wants")).toArray();
    for (const QJsonValue &want : wants) {
        if (want.isString()) {
            info.wants.append(want.toString());
        }
    }

    out = info;
    return true;
}

QJsonObject makeHello(const HelloInfo &hello)
{
    QJsonObject display;
    display.insert(QStringLiteral("id"), hello.displayId);
    display.insert(QStringLiteral("w"), hello.width);
    display.insert(QStringLiteral("h"), hello.height);

    QJsonObject message = envelope(MessageType::HELLO);
    message.insert(QStringLiteral("client_id"), hello.clientId);
    message.insert(QStringLiteral("role"), hello.role);
    message.insert(QStringLiteral("display"), display);
    message.insert(QStringLiteral("wants"), QJsonArray::fromStringList(hello.wants));
    return message;
}

QJsonObject makeSnapshot(const QJsonObject &session, const QJsonObject &vehicle)
{
    QJsonObject message = envelope(MessageType::SNAPSHOT);
    message.insert(QStringLiteral("session"), session);
    message.insert(QStringLiteral("vehicle"), vehicle);
    return message;
}

QJsonObject makeVehicle(const QJsonObject &values)
{
    QJsonObject message = envelope(MessageType::VEHICLE);
    message.insert(QStringLiteral("values"), values);
    return message;
}

QJsonObject makeVehicleEvent(const QString &name)
{
    QJsonObject message = envelope(MessageType::VEHICLE_EVENT);
    message.insert(QStringLiteral("event"), name);
    return message;
}

QJsonObject makeVehicleSet(const QString &group, int action, const QJsonObject &values)
{
    QJsonObject message = envelope(MessageType::VEHICLE_SET);
    message.insert(QStringLiteral("group"), group);
    message.insert(QStringLiteral("action"), action);
    message.insert(QStringLiteral("values"), values);
    return message;
}

QJsonObject makeError(const QString &code, const QString &text)
{
    QJsonObject message = envelope(MessageType::ERROR_MESSAGE);
    message.insert(QStringLiteral("code"), code);
    message.insert(QStringLiteral("text"), text);
    return message;
}

QString toText(const QJsonObject &message)
{
    return QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

} // namespace Protocol
