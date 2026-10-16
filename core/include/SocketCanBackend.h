#pragma once

#include "CanInterface.h"
#include <memory>

class QSocketNotifier;

class SocketCanBackend : public CanInterface
{
    Q_OBJECT

public:
    explicit SocketCanBackend(QObject *parent = nullptr);
    ~SocketCanBackend() override;

    bool open(const QString &interfaceName = QString("vcan0")) override;
    void close() override;
    bool isOpen() const override;
    bool sendFrame(const CanFrame &frame) override;

private slots:
    void onSocketActivated();

private:
    int m_socketFd{-1};
    QString m_interfaceName;
    QSocketNotifier *m_notifier{nullptr};
};
