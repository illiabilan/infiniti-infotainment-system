#pragma once

#include "CanFrame.h"
#include <QObject>
#include <QString>

class CanInterface : public QObject
{
    Q_OBJECT

public:
    explicit CanInterface(QObject *parent = nullptr) : QObject(parent) {}
    ~CanInterface() override = default;

    virtual bool open(const QString &interfaceName = QString()) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual bool sendFrame(const CanFrame &frame) = 0;

signals:
    void frameReceived(const CanFrame &frame);
    void errorOccurred(const QString &errorString);
};
