/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterTelltaleController.cpp
 * Author:         SK Rehan Ahamed
 * Description:    Telltales, Lighting, and TPMS Controller Implementation
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#include "ClusterTelltaleController.h"
#include <algorithm>
#include <cmath>

ClusterTelltaleController::ClusterTelltaleController(QObject *parent)
    : QObject(parent)
{
    m_lightPopupTimer.setSingleShot(true);
    connect(&m_lightPopupTimer, &QTimer::timeout, this, &ClusterTelltaleController::onLightPopupTimeout);

    m_tpmsDismissTimer.setSingleShot(true);
    connect(&m_tpmsDismissTimer, &QTimer::timeout, this, &ClusterTelltaleController::onTpmsDismissTimeout);
}

void ClusterTelltaleController::setParkBrakeActive(bool active)
{
    if (m_parkBrakeActive != active) {
        m_parkBrakeActive = active;
        emit parkBrakeActiveChanged();
    }
}

void ClusterTelltaleController::setSeatbeltActive(bool active)
{
    if (m_seatbeltActive != active) {
        m_seatbeltActive = active;
        emit seatbeltActiveChanged();
    }
}

void ClusterTelltaleController::setAbsActive(bool active)
{
    if (m_absActive != active) {
        m_absActive = active;
        emit absActiveChanged();
    }
}

void ClusterTelltaleController::setTpmsActive(bool active)
{
    if (m_tpmsActive != active) {
        m_tpmsActive = active;
        emit tpmsActiveChanged();
    }
}

void ClusterTelltaleController::setBatteryActive(bool active)
{
    if (m_batteryActive != active) {
        m_batteryActive = active;
        emit batteryActiveChanged();
    }
}

void ClusterTelltaleController::setOilActive(bool active)
{
    if (m_oilActive != active) {
        m_oilActive = active;
        emit oilActiveChanged();
    }
}

void ClusterTelltaleController::setLeftIndicator(bool active)
{
    if (m_leftIndicator != active) {
        m_leftIndicator = active;
        emit leftIndicatorChanged();
    }
}

void ClusterTelltaleController::setRightIndicator(bool active)
{
    if (m_rightIndicator != active) {
        m_rightIndicator = active;
        emit rightIndicatorChanged();
    }
}

void ClusterTelltaleController::setHighBeam(bool active)
{
    if (m_highBeam != active) {
        m_highBeam = active;
        emit highBeamChanged();
    }
}

void ClusterTelltaleController::setLowBeam(bool active)
{
    if (m_lowBeam != active) {
        m_lowBeam = active;
        emit lowBeamChanged();
    }
}

void ClusterTelltaleController::setPositionLamp(bool active)
{
    if (m_positionLamp != active) {
        m_positionLamp = active;
        emit positionLampChanged();
    }
}

void ClusterTelltaleController::setMasterWarning(bool active)
{
    if (m_masterWarning != active) {
        m_masterWarning = active;
        emit masterWarningChanged();
    }
}

void ClusterTelltaleController::setLightWarning(bool active)
{
    if (m_lightWarning != active) {
        m_lightWarning = active;
        emit lightWarningChanged();
    }
}

void ClusterTelltaleController::setEscActive(bool active)
{
    if (m_escActive != active) {
        m_escActive = active;
        emit escActiveChanged();
    }
}

void ClusterTelltaleController::setEscOffActive(bool active)
{
    if (m_escOffActive != active) {
        m_escOffActive = active;
        emit escOffActiveChanged();
    }
}

void ClusterTelltaleController::setCheckEngineActive(bool active)
{
    if (m_checkEngineActive != active) {
        m_checkEngineActive = active;
        emit checkEngineActiveChanged();
    }
}

void ClusterTelltaleController::setSmartKeyActive(bool active)
{
    if (m_smartKeyActive != active) {
        m_smartKeyActive = active;
        emit smartKeyActiveChanged();
    }
}

void ClusterTelltaleController::setSteeringActive(bool active)
{
    if (m_steeringActive != active) {
        m_steeringActive = active;
        emit steeringActiveChanged();
    }
}

void ClusterTelltaleController::setAirbagActive(bool active)
{
    if (m_airbagActive != active) {
        m_airbagActive = active;
        emit airbagActiveChanged();
    }
}

void ClusterTelltaleController::setAllTelltales(bool active)
{
    setParkBrakeActive(active);
    setSeatbeltActive(active);
    setAbsActive(active);
    setTpmsActive(active);
    setBatteryActive(active);
    setOilActive(active);
    setLeftIndicator(active);
    setRightIndicator(active);
    setHighBeam(active);
    setLowBeam(active);
    setPositionLamp(active);
    setMasterWarning(active);
    setLightWarning(active);
    setEscActive(active);
    setEscOffActive(active);
    setCheckEngineActive(active);
    setSmartKeyActive(active);
    setSteeringActive(active);
    setAirbagActive(active);
}

void ClusterTelltaleController::setBulbCheckTelltales(bool active)
{
    setParkBrakeActive(active);
    setSeatbeltActive(active);
    setAbsActive(active);
    setTpmsActive(active);
    setSmartKeyActive(active);
    setMasterWarning(active);
    setSteeringActive(active);
    setBatteryActive(active);
    setAirbagActive(active);
    setOilActive(active);
    setEscActive(active);
    setEscOffActive(active);
    setCheckEngineActive(active);

    setLeftIndicator(false);
    setRightIndicator(false);
    setHighBeam(false);
    setLowBeam(m_lightMode == 3);
    setPositionLamp(m_lightMode == 2 || m_lightMode == 3);
    setLightWarning(false);
}

void ClusterTelltaleController::applyNormalTripTelltales()
{
    setAbsActive(false);
    setAirbagActive(false);
    setTpmsActive(false);
    setEscActive(false);
    setSteeringActive(false);
    setSeatbeltActive(false);

    setParkBrakeActive(true);
    setBatteryActive(true);
    setOilActive(true);
    setCheckEngineActive(true);
    setSmartKeyActive(false);
    setEscOffActive(false);
    setMasterWarning(false);
}

void ClusterTelltaleController::setShowLightPopup(bool show)
{
    if (m_showLightPopup != show) {
        m_showLightPopup = show;
        emit showLightPopupChanged();
    }
    if (m_showLightPopup) {
        m_lightPopupTimer.start(3000);
    } else {
        m_lightPopupTimer.stop();
    }
}

void ClusterTelltaleController::setLightMode(int mode)
{
    int clamped = std::clamp(mode, 0, 3);
    bool changed = (m_lightMode != clamped);
    m_lightMode = clamped;
    if (changed) {
        emit lightModeChanged();
        setShowLightPopup(true);
    }

    switch (m_lightMode) {
    case 0:
        setPositionLamp(false);
        setLowBeam(false);
        break;
    case 1:
        setPositionLamp(true);
        setLowBeam(true);
        break;
    case 2:
        setPositionLamp(true);
        setLowBeam(false);
        break;
    case 3:
        setPositionLamp(true);
        setLowBeam(true);
        break;
    }
}

void ClusterTelltaleController::cycleLightMode()
{
    setLightMode((m_lightMode + 1) % 4);
}

void ClusterTelltaleController::updateTyrePsi(double &member, double psi, void (ClusterTelltaleController::*changedSignal)())
{
    if (std::abs(member - psi) > 0.1) {
        member = psi;
        emit (this->*changedSignal)();
        updateTpmsLowWarning();
    }
}

void ClusterTelltaleController::updateTpmsLowWarning()
{
    bool anyLow = (m_flPsi < 32.0 || m_frPsi < 32.0 || m_rlPsi < 32.0 || m_rrPsi < 32.0);
    setTpmsActive(anyLow);
}

void ClusterTelltaleController::setFlPsi(double psi)
{
    updateTyrePsi(m_flPsi, psi, &ClusterTelltaleController::flPsiChanged);
}

void ClusterTelltaleController::setFrPsi(double psi)
{
    updateTyrePsi(m_frPsi, psi, &ClusterTelltaleController::frPsiChanged);
}

void ClusterTelltaleController::setRlPsi(double psi)
{
    updateTyrePsi(m_rlPsi, psi, &ClusterTelltaleController::rlPsiChanged);
}

void ClusterTelltaleController::setRrPsi(double psi)
{
    updateTyrePsi(m_rrPsi, psi, &ClusterTelltaleController::rrPsiChanged);
}

void ClusterTelltaleController::setAllPsi(double psi)
{
    double clamped = std::clamp(psi, 15.0, 50.0);
    setFlPsi(clamped);
    setFrPsi(clamped);
    setRlPsi(clamped);
    setRrPsi(clamped);
}

void ClusterTelltaleController::adjustPsi(const QString &tyre, double delta)
{
    if (tyre.compare(QString::fromUtf8("FL"), Qt::CaseInsensitive) == 0) {
        setFlPsi(std::clamp(m_flPsi + delta, 15.0, 50.0));
    } else if (tyre.compare(QString::fromUtf8("FR"), Qt::CaseInsensitive) == 0) {
        setFrPsi(std::clamp(m_frPsi + delta, 15.0, 50.0));
    } else if (tyre.compare(QString::fromUtf8("RL"), Qt::CaseInsensitive) == 0) {
        setRlPsi(std::clamp(m_rlPsi + delta, 15.0, 50.0));
    } else if (tyre.compare(QString::fromUtf8("RR"), Qt::CaseInsensitive) == 0) {
        setRrPsi(std::clamp(m_rrPsi + delta, 15.0, 50.0));
    } else if (tyre.compare(QString::fromUtf8("ALL"), Qt::CaseInsensitive) == 0) {
        setFlPsi(std::clamp(m_flPsi + delta, 15.0, 50.0));
        setFrPsi(std::clamp(m_frPsi + delta, 15.0, 50.0));
        setRlPsi(std::clamp(m_rlPsi + delta, 15.0, 50.0));
        setRrPsi(std::clamp(m_rrPsi + delta, 15.0, 50.0));
    }
}

void ClusterTelltaleController::resetTpms()
{
    setAllPsi(35.0);
    setTpmsCalibrated(true);
}

void ClusterTelltaleController::setTpmsCalibrated(bool cal)
{
    if (m_tpmsCalibrated != cal) {
        m_tpmsCalibrated = cal;
        emit tpmsCalibratedChanged();
    }
}

void ClusterTelltaleController::startTpmsDismissTimer()
{
    m_tpmsDismissTimer.start(5000);
}

void ClusterTelltaleController::onLightPopupTimeout()
{
    setShowLightPopup(false);
}

void ClusterTelltaleController::onTpmsDismissTimeout()
{
    emit tpmsDismissTimeout();
}
