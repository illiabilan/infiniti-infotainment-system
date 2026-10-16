#pragma once

#include "CanInterface.h"
#include <QUdpSocket>
#include <QHostAddress>

class MockCanBackend : public CanInterface
{
    Q_OBJECT

public:
    static constexpr quint16 DEFAULT_PORT = 45454;
    static constexpr const char* DEFAULT_MULTICAST_GROUP = "239.255.45.54";

    explicit MockCanBackend(QObject *parent = nullptr);
    ~MockCanBackend() override;

    bool open(const QString &interfaceName = QString()) override;
    void close() override;
    bool isOpen() const override;
    bool sendFrame(const CanFrame &frame) override;

    void setMulticastGroup(const QString &groupAddress);
    void setPort(quint16 port);

private slots:
    void onReadyRead();

private:
    QUdpSocket *m_socket{nullptr};
    QHostAddress m_multicastGroup{DEFAULT_MULTICAST_GROUP};
    quint16 m_port{DEFAULT_PORT};
    bool m_isOpen{false};
};
