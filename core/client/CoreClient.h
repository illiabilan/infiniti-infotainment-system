#pragma once

#include "Protocol.h"
#include "VehicleChangeWatcher.h"
#include "VehicleModel.h"
#include <QJsonObject>
#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QWebSocket>

// Client side of the Core protocol, shared by the display apps. Fills a
// VehicleModel mirror from Core and forwards local publish requests to Core.
// The mirror must outlive the client.
class CoreClient : public QObject
{
    Q_OBJECT

public:
    static constexpr int RETRY_INTERVAL_MS = 1000;

    explicit CoreClient(VehicleModel *mirror, QObject *parent = nullptr);

    bool start(const Protocol::HelloInfo &hello);
    bool isConnected() const { return m_socket.state() == QAbstractSocket::ConnectedState; }

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString &text);
    void onLocalValues(const QJsonObject &values);
    void onPublishRequested(VehicleModel::Group group, quint8 action);

private:
    void applyRemote(const QJsonObject &values);

    VehicleModel *m_mirror;
    VehicleChangeWatcher m_watcher;
    QWebSocket m_socket;
    QTimer m_retryTimer;
    Protocol::HelloInfo m_hello;
    QUrl m_url;
    QJsonObject m_pending;
    bool m_applyingRemote = false;
};
