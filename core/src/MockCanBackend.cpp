#include "MockCanBackend.h"
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QVariant>
#include <QDebug>

MockCanBackend::MockCanBackend(QObject *parent)
    : CanInterface(parent)
{
}

MockCanBackend::~MockCanBackend()
{
    close();
}

void MockCanBackend::setMulticastGroup(const QString &groupAddress)
{
    m_multicastGroup = QHostAddress(groupAddress);
}

void MockCanBackend::setPort(quint16 port)
{
    m_port = port;
}

bool MockCanBackend::open(const QString &interfaceName)
{
    Q_UNUSED(interfaceName);
    close();

    m_socket = new QUdpSocket(this);

    // Bind socket sharing address and port for multi-process multicast subscription
    bool bound = m_socket->bind(QHostAddress::AnyIPv4, m_port,
                                QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    if (!bound) {
        emit errorOccurred(QString("Failed to bind UDP socket on port %1: %2").arg(m_port).arg(m_socket->errorString()));
        delete m_socket;
        m_socket = nullptr;
        return false;
    }

    // Locate loopback interface for local multi-process CAN frame distribution
    QNetworkInterface loopback = QNetworkInterface::interfaceFromName("lo0");
    if (!loopback.isValid()) {
        const auto ifaces = QNetworkInterface::allInterfaces();
        for (const auto &iface : ifaces) {
            if (iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                loopback = iface;
                break;
            }
        }
    }

    if (loopback.isValid()) {
        m_socket->setMulticastInterface(loopback);
        if (!m_socket->joinMulticastGroup(m_multicastGroup, loopback)) {
            emit errorOccurred(QString("Failed to join multicast group %1 on loopback: %2")
                                   .arg(m_multicastGroup.toString(), m_socket->errorString()));
            delete m_socket;
            m_socket = nullptr;
            return false;
        }
    } else {
        if (!m_socket->joinMulticastGroup(m_multicastGroup)) {
            emit errorOccurred(QString("Failed to join multicast group %1: %2")
                                   .arg(m_multicastGroup.toString(), m_socket->errorString()));
            delete m_socket;
            m_socket = nullptr;
            return false;
        }
    }

    m_socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);

    connect(m_socket, &QUdpSocket::readyRead, this, &MockCanBackend::onReadyRead);

    m_isOpen = true;
    qInfo() << "MockCanBackend listening on multicast" << m_multicastGroup.toString() << ":" << m_port;
    return true;
}

void MockCanBackend::close()
{
    if (m_socket) {
        if (m_isOpen) {
            m_socket->leaveMulticastGroup(m_multicastGroup);
        }
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }
    m_isOpen = false;
}

bool MockCanBackend::isOpen() const
{
    return m_isOpen && m_socket != nullptr;
}

bool MockCanBackend::sendFrame(const CanFrame &frame)
{
    if (!m_isOpen || !m_socket) {
        emit errorOccurred("Mock CAN socket is not open");
        return false;
    }

    QByteArray datagram(reinterpret_cast<const char*>(&frame), sizeof(CanFrame));
    qint64 written = m_socket->writeDatagram(datagram, m_multicastGroup, m_port);
    if (written != sizeof(CanFrame)) {
        qWarning() << "Failed to send CAN frame datagram:" << m_socket->errorString();
        emit errorOccurred(QString("Failed to send CAN frame datagram: %1").arg(m_socket->errorString()));
        return false;
    }

    return true;
}

void MockCanBackend::onReadyRead()
{
    while (m_socket && m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        QByteArray data = datagram.data();
        if (data.size() >= static_cast<int>(sizeof(CanFrame))) {
            const auto *framePtr = reinterpret_cast<const CanFrame*>(data.constData());
            emit frameReceived(*framePtr);
        }
    }
}
