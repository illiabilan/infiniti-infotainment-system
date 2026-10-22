#pragma once

#include "UsbDeviceInfo.h"
#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QTimer>

class AoaUsbPort;

// The Z1 state machine (docs/tickets/Z1-aoa-contract.md). Runs on the Qt thread only.
// It does not touch AaSessionState and emits no signals. Failures are logged and the state becomes Failed.
class AoaSwitcher : public QObject
{
    Q_OBJECT

public:
    enum class State { Idle, Switching, AccessoryUp, Failed };

    static constexpr int SWITCH_TIMEOUT_MS = 2000;

    // Contract names, not wire strings
    static QString stateName(State state);

    // The port is not owned and must outlive the switcher
    explicit AoaSwitcher(AoaUsbPort *port, QObject *parent = nullptr);

    bool start();

    State state() const { return m_state; }

private slots:
    void onDeviceArrived(const UsbDeviceInfo &info, bool atStartup);
    void onDeviceLeft(quint64 id);
    void onAccessoryModeRequested(quint64 id);
    void onAccessoryModeFailed(quint64 id, const QString &errorText);
    void onSwitchTimeout();

private:
    void resetLeftover(const UsbDeviceInfo &info);
    void beginSwitch(const UsbDeviceInfo &info);
    void openAccessory(const UsbDeviceInfo &info);
    void fail(const QString &reason);

    AoaUsbPort *m_port;
    State m_state{State::Idle};
    // Meaningful only when m_state is not Idle
    quint64 m_deviceId{0};
    QString m_label;
    QTimer m_switchTimer;
    QElapsedTimer m_sinceCandidate;
};
