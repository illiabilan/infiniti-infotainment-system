#include "AoaSwitcher.h"

#include "AoaDeviceFilter.h"
#include "AoaMarkers.h"
#include "AoaUsbPort.h"
#include <QDebug>

QString AoaSwitcher::stateName(State state)
{
    switch (state) {
    case State::Idle: return QStringLiteral("Idle");
    case State::Switching: return QStringLiteral("Switching");
    case State::AccessoryUp: return QStringLiteral("AccessoryUp");
    case State::Failed: return QStringLiteral("Failed");
    }
    return QStringLiteral("Idle");
}

AoaSwitcher::AoaSwitcher(AoaUsbPort *port, QObject *parent)
    : QObject(parent)
    , m_port(port)
{
    connect(m_port, &AoaUsbPort::deviceArrived, this, &AoaSwitcher::onDeviceArrived);
    connect(m_port, &AoaUsbPort::deviceLeft, this, &AoaSwitcher::onDeviceLeft);
    connect(m_port, &AoaUsbPort::accessoryModeRequested, this, &AoaSwitcher::onAccessoryModeRequested);
    connect(m_port, &AoaUsbPort::accessoryModeFailed, this, &AoaSwitcher::onAccessoryModeFailed);

    m_switchTimer.setSingleShot(true);
    m_switchTimer.setInterval(SWITCH_TIMEOUT_MS);
    connect(&m_switchTimer, &QTimer::timeout, this, &AoaSwitcher::onSwitchTimeout);
}

bool AoaSwitcher::start()
{
    QString errorText;
    if (!m_port->start(&errorText)) {
        qWarning().noquote() << AoaMarkers::line({}, AoaMarkers::WATCH_FAILED.arg(errorText));
        return false;
    }
    qInfo().noquote() << AoaMarkers::line({}, AoaMarkers::WATCHING);
    return true;
}

void AoaSwitcher::onDeviceArrived(const UsbDeviceInfo &info, bool atStartup)
{
    if (AoaDeviceFilter::isAccessory(info)) {
        if (atStartup) {
            resetLeftover(info);
        } else if (m_state == State::Switching || m_state == State::Failed) {
            openAccessory(info);
        }
        return;
    }
    if (AoaDeviceFilter::isCandidate(info) && (m_state == State::Idle || m_state == State::Failed)) {
        beginSwitch(info);
    }
}

void AoaSwitcher::onDeviceLeft(quint64 id)
{
    // In Switching the device leaving is the phone re-enumerating after the switch request
    if ((m_state == State::AccessoryUp || m_state == State::Failed) && id == m_deviceId) {
        m_state = State::Idle;
        qInfo().noquote() << AoaMarkers::line(m_label, AoaMarkers::CLOSED);
    }
}

void AoaSwitcher::onAccessoryModeRequested(quint64 id)
{
    if (m_state == State::Switching && id == m_deviceId) {
        qInfo().noquote() << AoaMarkers::line(m_label, AoaMarkers::REQUESTED);
    }
}

void AoaSwitcher::onAccessoryModeFailed(quint64 id, const QString &errorText)
{
    if (m_state == State::Switching && id == m_deviceId) {
        fail(errorText);
    }
}

void AoaSwitcher::onSwitchTimeout()
{
    if (m_state == State::Switching) {
        fail(AoaMarkers::NO_ACCESSORY.arg(QString::number(SWITCH_TIMEOUT_MS)));
    }
}

void AoaSwitcher::resetLeftover(const UsbDeviceInfo &info)
{
    const QString label = AoaMarkers::deviceLabel(info.serial);
    qInfo().noquote() << AoaMarkers::line(label, AoaMarkers::LEFTOVER);

    QString errorText;
    if (!m_port->resetDevice(info.id, &errorText)) {
        qWarning().noquote() << AoaMarkers::line(label, AoaMarkers::RESET_FAILED.arg(errorText));
    }
}

void AoaSwitcher::beginSwitch(const UsbDeviceInfo &info)
{
    // State first: the port must not see a stale state if it answers at once
    m_state = State::Switching;
    m_deviceId = info.id;
    m_label = AoaMarkers::deviceLabel(info.serial);
    m_sinceCandidate.start();
    m_switchTimer.start();
    qInfo().noquote() << AoaMarkers::line(
        m_label, AoaMarkers::CANDIDATE.arg(AoaMarkers::vidPid(info.vendorId, info.productId)));
    m_port->requestAccessoryMode(info.id);
}

void AoaSwitcher::openAccessory(const UsbDeviceInfo &info)
{
    m_switchTimer.stop();
    m_deviceId = info.id;
    m_label = AoaMarkers::deviceLabel(info.serial);

    QString errorText;
    if (!m_port->openAccessory(info.id, &errorText)) {
        fail(AoaMarkers::OPEN_FAILED.arg(errorText));
        return;
    }
    m_state = State::AccessoryUp;
    qInfo().noquote() << AoaMarkers::line(
        m_label, AoaMarkers::ACCESSORY_UP.arg(QString::number(m_sinceCandidate.elapsed())));
}

void AoaSwitcher::fail(const QString &reason)
{
    m_switchTimer.stop();
    m_state = State::Failed;
    qWarning().noquote() << AoaMarkers::line(m_label, AoaMarkers::AOAP_FAILED.arg(reason));
}
