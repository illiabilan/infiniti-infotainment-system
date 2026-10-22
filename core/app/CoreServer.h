#pragma once

#include "VehicleChangeWatcher.h"
#include "VehicleModel.h"
#include <QHostAddress>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QWebSocketServer>
#include <array>

class AaSessionState;
class ClientConnection;

// Serves protocol v1 on 127.0.0.1. Reads the AA session state and never writes it.
class CoreServer : public QObject
{
    Q_OBJECT

public:
    CoreServer(VehicleModel *vehicle, AaSessionState *session, QObject *parent = nullptr);

    bool listen(quint16 port);

    quint16 port() const { return m_server.serverPort(); }
    QHostAddress address() const { return m_server.serverAddress(); }
    QString errorString() const { return m_server.errorString(); }
    int clientCount() const { return static_cast<int>(m_clients.size()); }

private slots:
    void onNewConnection();
    void onVehicleValuesChanged(const QJsonObject &values);
    void onVehicleEvent(const QString &name);
    void sendHeartbeat();

private:
    static constexpr std::array<VehicleModel::Group, 5> ALLOWED_CLIENT_GROUPS{
        VehicleModel::Group::Climate,
        VehicleModel::Group::BodyClosures,
        VehicleModel::Group::Lighting,
        VehicleModel::Group::CoolantFuel,
        VehicleModel::Group::InfotainmentMfd
    };

    void dispatch(ClientConnection *connection, const QJsonObject &message);
    void handleHello(ClientConnection *connection, const QJsonObject &message);
    void handleVehicleSet(ClientConnection *connection, const QJsonObject &message);
    void sendToVehicleSubscribers(const QJsonObject &message);
    void onClientDisconnected(ClientConnection *connection);
    QString sessionLabel() const;

    QWebSocketServer m_server;
    QList<ClientConnection *> m_clients;
    VehicleModel *m_vehicle;
    AaSessionState *m_session;
    VehicleChangeWatcher m_watcher;
    QTimer m_heartbeat;
};
