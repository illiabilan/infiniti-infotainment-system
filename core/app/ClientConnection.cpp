#include "ClientConnection.h"
#include <QDebug>
#include <QWebSocket>

ClientConnection::ClientConnection(QWebSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
{
    m_socket->setParent(this);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &ClientConnection::onTextMessage);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &ClientConnection::onBinaryMessage);
    connect(m_socket, &QWebSocket::disconnected, this, &ClientConnection::disconnected);
}

void ClientConnection::send(const QJsonObject &message)
{
    m_socket->sendTextMessage(Protocol::toText(message));
}

void ClientConnection::sendBinary(const QByteArray &data)
{
    m_socket->sendBinaryMessage(data);
}

bool ClientConnection::wants(QLatin1StringView want) const
{
    return m_hello.has_value() && m_hello->wants.contains(want);
}

QString ClientConnection::label() const
{
    return m_hello.has_value() ? m_hello->clientId : QStringLiteral("-");
}

void ClientConnection::onTextMessage(const QString &text)
{
    const Protocol::ParseResult result = Protocol::parse(text);
    if (!result.ok) {
        send(Protocol::makeError(result.errorCode, result.errorText));
        qWarning().noquote() << QStringLiteral("core: rejected id=%1 code=%2").arg(label(), result.errorCode);
        return;
    }
    emit messageReceived(result.message);
}

void ClientConnection::onBinaryMessage(const QByteArray &)
{
    send(Protocol::makeError(Protocol::ErrorCode::INVALID_MESSAGE, QStringLiteral("binary messages are not accepted")));
    qWarning().noquote() << QStringLiteral("core: rejected id=%1 code=%2")
                                .arg(label(), QString(Protocol::ErrorCode::INVALID_MESSAGE));
}
