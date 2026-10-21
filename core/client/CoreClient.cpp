#include "CoreClient.h"
#include "CoreEndpointConfig.h"
#include "VehicleJson.h"
#include <QDebug>
#include <QMetaEnum>

CoreClient::CoreClient(VehicleModel *mirror, QObject *parent)
    : QObject(parent)
    , m_mirror(mirror)
    , m_watcher(mirror)
{
    connect(&m_watcher, &VehicleChangeWatcher::valuesChanged, this, &CoreClient::onLocalValues);
    connect(m_mirror, &VehicleModel::publishRequested, this, &CoreClient::onPublishRequested);

    connect(&m_socket, &QWebSocket::connected, this, &CoreClient::onConnected);
    connect(&m_socket, &QWebSocket::disconnected, this, &CoreClient::onDisconnected);
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &CoreClient::onTextMessage);

    m_retryTimer.setInterval(RETRY_INTERVAL_MS);
    connect(&m_retryTimer, &QTimer::timeout, this, [this]() {
        if (m_socket.state() == QAbstractSocket::UnconnectedState) {
            m_socket.open(m_url);
        }
    });
}

bool CoreClient::start(const Protocol::HelloInfo &hello)
{
    m_hello = hello;

    QString error;
    const std::optional<quint16> port = CoreEndpointConfig::loadPort(&error);
    if (!port) {
        qCritical().noquote() << QStringLiteral("core client: %1").arg(error);
        return false;
    }

    m_url = CoreEndpointConfig::serverUrl(*port);
    m_socket.open(m_url);
    m_retryTimer.start();
    return true;
}

void CoreClient::onConnected()
{
    m_retryTimer.stop();
    m_socket.sendTextMessage(Protocol::toText(Protocol::makeHello(m_hello)));
    qInfo().noquote() << QStringLiteral("core client: connected %1 id=%2").arg(m_url.toString(), m_hello.clientId);
}

void CoreClient::onDisconnected()
{
    qWarning() << "core client: disconnected, retrying";
    m_pending = QJsonObject();
    m_retryTimer.start();
}

void CoreClient::onTextMessage(const QString &text)
{
    const Protocol::ParseResult result = Protocol::parse(text);
    if (!result.ok) {
        qWarning().noquote() << QStringLiteral("core client: bad message %1: %2").arg(result.errorCode, result.errorText);
        return;
    }

    const QString type = Protocol::typeOf(result.message);
    if (type == Protocol::MessageType::SNAPSHOT) {
        applyRemote(result.message.value(QStringLiteral("vehicle")).toObject());
    } else if (type == Protocol::MessageType::VEHICLE) {
        applyRemote(result.message.value(QStringLiteral("values")).toObject());
    } else if (type == Protocol::MessageType::VEHICLE_EVENT) {
        const QString name = result.message.value(QStringLiteral("event")).toString();
        if (!VehicleJson::raiseEvent(*m_mirror, name)) {
            qWarning().noquote() << QStringLiteral("core client: unknown vehicle event %1").arg(name);
        }
    } else if (type == Protocol::MessageType::ERROR_MESSAGE) {
        qWarning().noquote() << QStringLiteral("core client: error %1: %2")
                                    .arg(result.message.value(QStringLiteral("code")).toString(),
                                         result.message.value(QStringLiteral("text")).toString());
    }
}

void CoreClient::applyRemote(const QJsonObject &values)
{
    m_applyingRemote = true;
    VehicleJson::applyJson(*m_mirror, values);
    m_applyingRemote = false;
}

void CoreClient::onLocalValues(const QJsonObject &values)
{
    if (m_applyingRemote) return;

    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        m_pending.insert(it.key(), it.value());
    }
}

void CoreClient::onPublishRequested(VehicleModel::Group group, quint8 action)
{
    const QString groupName = QString::fromLatin1(QMetaEnum::fromType<VehicleModel::Group>().valueToKey(static_cast<int>(group)));

    if (!isConnected()) {
        qWarning().noquote() << QStringLiteral("core client: not connected, dropped %1").arg(groupName);
        m_pending = QJsonObject();
        return;
    }

    m_socket.sendTextMessage(Protocol::toText(Protocol::makeVehicleSet(groupName, action, m_pending)));
    m_pending = QJsonObject();
}
