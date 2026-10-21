#pragma once

#include "Protocol.h"
#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <optional>

class QWebSocket;

// One connected client. Owns its socket and validates the message envelope.
class ClientConnection : public QObject
{
    Q_OBJECT

public:
    explicit ClientConnection(QWebSocket *socket, QObject *parent);

    void send(const QJsonObject &message);
    void sendBinary(const QByteArray &data);

    bool isGreeted() const { return m_hello.has_value(); }
    const Protocol::HelloInfo &hello() const { return *m_hello; }
    void setHello(const Protocol::HelloInfo &hello) { m_hello = hello; }
    bool wants(QLatin1StringView want) const;

    // client_id, or "-" before hello. Used in log markers.
    QString label() const;

signals:
    void messageReceived(const QJsonObject &message);
    void disconnected();

private slots:
    void onTextMessage(const QString &text);
    void onBinaryMessage(const QByteArray &data);

private:
    QWebSocket *m_socket;
    std::optional<Protocol::HelloInfo> m_hello;
};
