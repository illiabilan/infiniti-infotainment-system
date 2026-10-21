#include "CoreServer.h"
#include "AaSessionState.h"
#include "ClientConnection.h"
#include "Protocol.h"
#include "VehicleJson.h"
#include <QDebug>
#include <QMetaEnum>
#include <QWebSocket>
#include <algorithm>
#include <cmath>

static constexpr int HEARTBEAT_INTERVAL_MS = 1000;

CoreServer::CoreServer(VehicleModel *vehicle, AaSessionState *session, QObject *parent)
    : QObject(parent)
    , m_server(QStringLiteral("Infiniti Core"), QWebSocketServer::NonSecureMode, this)
    , m_vehicle(vehicle)
    , m_session(session)
    , m_watcher(vehicle)
{
    connect(&m_watcher, &VehicleChangeWatcher::valuesChanged, this, &CoreServer::onVehicleValuesChanged);
    connect(&m_watcher, &VehicleChangeWatcher::eventRaised, this, &CoreServer::onVehicleEvent);
    connect(&m_server, &QWebSocketServer::newConnection, this, &CoreServer::onNewConnection);

    m_heartbeat.setInterval(HEARTBEAT_INTERVAL_MS);
    connect(&m_heartbeat, &QTimer::timeout, this, &CoreServer::sendHeartbeat);
}

bool CoreServer::listen(quint16 port)
{
    if (!m_server.listen(QHostAddress::LocalHost, port)) {
        return false;
    }
    m_heartbeat.start();
    return true;
}

void CoreServer::onNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QWebSocket *socket = m_server.nextPendingConnection();
        auto *connection = new ClientConnection(socket, this);
        m_clients.append(connection);

        connect(connection, &ClientConnection::messageReceived, this,
                [this, connection](const QJsonObject &message) { dispatch(connection, message); });
        connect(connection, &ClientConnection::disconnected, this,
                [this, connection]() { onClientDisconnected(connection); });

        qInfo().noquote() << QStringLiteral("core: client connected peer=%1:%2")
                                 .arg(socket->peerAddress().toString())
                                 .arg(socket->peerPort());
    }
}

void CoreServer::dispatch(ClientConnection *connection, const QJsonObject &message)
{
    const QString type = Protocol::typeOf(message);
    if (type == Protocol::MessageType::HELLO) {
        handleHello(connection, message);
    } else if (type == Protocol::MessageType::VEHICLE_SET) {
        handleVehicleSet(connection, message);
    } else {
        connection->send(Protocol::makeError(Protocol::ErrorCode::UNKNOWN_TYPE,
                                             QStringLiteral("unknown type: %1").arg(type)));
    }
}

void CoreServer::handleHello(ClientConnection *connection, const QJsonObject &message)
{
    Protocol::HelloInfo info;
    QString errorText;
    if (!Protocol::parseHello(message, info, errorText)) {
        connection->send(Protocol::makeError(Protocol::ErrorCode::INVALID_HELLO, errorText));
        return;
    }
    connection->setHello(info);

    connection->send(Protocol::makeSnapshot(m_session->toJson(), VehicleJson::toJson(*m_vehicle)));

    const QByteArray keyframe = m_session->keyframe();
    const bool sendKeyframe = connection->wants(Protocol::Want::VIDEO) && !keyframe.isEmpty();
    if (sendKeyframe) {
        connection->sendBinary(keyframe);
    }

    qInfo().noquote() << QStringLiteral("core: hello id=%1 role=%2 aa_session=%3 keyframe=%4")
                             .arg(info.clientId, info.role, sessionLabel(),
                                  sendKeyframe ? QStringLiteral("sent") : QStringLiteral("none"));
}

void CoreServer::handleVehicleSet(ClientConnection *connection, const QJsonObject &message)
{
    if (!connection->isGreeted()) {
        connection->send(Protocol::makeError(Protocol::ErrorCode::NOT_GREETED,
                                             QStringLiteral("send hello first")));
        return;
    }

    const QString groupName = message.value(QStringLiteral("group")).toString();
    bool groupOk = false;
    const int groupValue = QMetaEnum::fromType<VehicleModel::Group>().keyToValue(groupName.toLatin1().constData(), &groupOk);
    if (!groupOk) {
        connection->send(Protocol::makeError(Protocol::ErrorCode::INVALID_MESSAGE,
                                             QStringLiteral("unknown group: %1").arg(groupName)));
        return;
    }

    const auto group = static_cast<VehicleModel::Group>(groupValue);
    if (std::find(ALLOWED_CLIENT_GROUPS.begin(), ALLOWED_CLIENT_GROUPS.end(), group) == ALLOWED_CLIENT_GROUPS.end()) {
        connection->send(Protocol::makeError(Protocol::ErrorCode::FORBIDDEN_GROUP,
                                             QStringLiteral("group not allowed for clients: %1").arg(groupName)));
        return;
    }

    int action = 0;
    if (message.contains(QStringLiteral("action"))) {
        const QJsonValue actionValue = message.value(QStringLiteral("action"));
        const double raw = actionValue.toDouble(-1.0);
        if (!actionValue.isDouble() || raw < 0.0 || raw > 255.0 || raw != std::floor(raw)) {
            connection->send(Protocol::makeError(Protocol::ErrorCode::INVALID_MESSAGE,
                                                 QStringLiteral("action must be an integer from 0 to 255")));
            return;
        }
        action = static_cast<int>(raw);
    }

    QJsonObject values;
    if (message.contains(QStringLiteral("values"))) {
        const QJsonValue valuesValue = message.value(QStringLiteral("values"));
        if (!valuesValue.isObject()) {
            connection->send(Protocol::makeError(Protocol::ErrorCode::INVALID_MESSAGE,
                                                 QStringLiteral("values must be an object")));
            return;
        }
        values = valuesValue.toObject();
    }

    VehicleJson::applyJson(*m_vehicle, values);
    m_vehicle->publish(group, static_cast<quint8>(action));
}

void CoreServer::onVehicleValuesChanged(const QJsonObject &values)
{
    sendToVehicleSubscribers(Protocol::makeVehicle(values));
}

void CoreServer::onVehicleEvent(const QString &name)
{
    sendToVehicleSubscribers(Protocol::makeVehicleEvent(name));
}

void CoreServer::sendHeartbeat()
{
    sendToVehicleSubscribers(Protocol::makeVehicle(VehicleJson::toJson(*m_vehicle)));
}

void CoreServer::sendToVehicleSubscribers(const QJsonObject &message)
{
    for (ClientConnection *connection : std::as_const(m_clients)) {
        if (connection->wants(Protocol::Want::VEHICLE)) {
            connection->send(message);
        }
    }
}

void CoreServer::onClientDisconnected(ClientConnection *connection)
{
    m_clients.removeOne(connection);
    connection->deleteLater();

    const QString role = connection->isGreeted() ? connection->hello().role : QStringLiteral("-");
    qInfo().noquote() << QStringLiteral("core: client disconnected id=%1 role=%2 aa_session=%3 kept")
                             .arg(connection->label(), role, sessionLabel());
}

QString CoreServer::sessionLabel() const
{
    return QStringLiteral("%1/%2").arg(m_session->sessionId()).arg(AaSessionState::phaseName(m_session->phase()));
}
