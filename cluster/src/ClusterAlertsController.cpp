/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterAlertsController.cpp
 * Author:         SK Rehan Ahamed
 * Description:    Alerts, Closures, Seatbelts, and Cruise Controller Implementation
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#include "ClusterAlertsController.h"
#include <algorithm>

ClusterAlertsController::ClusterAlertsController(QObject *parent)
    : QObject(parent)
{
    m_rearAlarmTimer.setSingleShot(true);
    connect(&m_rearAlarmTimer, &QTimer::timeout, this, &ClusterAlertsController::onRearAlarmTimeout);

    m_rearBlinkTimer.setInterval(350);
    connect(&m_rearBlinkTimer, &QTimer::timeout, this, &ClusterAlertsController::onRearBlinkTick);

    m_reduceSpeedDismissTimer.setSingleShot(true);
    connect(&m_reduceSpeedDismissTimer, &QTimer::timeout, this, &ClusterAlertsController::onReduceSpeedDismissTimeout);

    m_overspeedBeepTimer.setInterval(1000);
    connect(&m_overspeedBeepTimer, &QTimer::timeout, this, &ClusterAlertsController::onOverspeedBeepTick);
}

void ClusterAlertsController::updateClosureState(bool &member, bool open, void (ClusterAlertsController::*changedSignal)())
{
    if (member != open) {
        member = open;
        m_doorOpenAlert = isAnyDoorOpen();
        emit (this->*changedSignal)();
        emit doorOpenAlertChanged();
        if (open) emit playChimeRequested();
    }
}

void ClusterAlertsController::setDoorFrontLeft(bool open)
{
    updateClosureState(m_doorFrontLeft, open, &ClusterAlertsController::doorFrontLeftChanged);
}

void ClusterAlertsController::setDoorFrontRight(bool open)
{
    updateClosureState(m_doorFrontRight, open, &ClusterAlertsController::doorFrontRightChanged);
}

void ClusterAlertsController::setDoorRearLeft(bool open)
{
    updateClosureState(m_doorRearLeft, open, &ClusterAlertsController::doorRearLeftChanged);
}

void ClusterAlertsController::setDoorRearRight(bool open)
{
    updateClosureState(m_doorRearRight, open, &ClusterAlertsController::doorRearRightChanged);
}

void ClusterAlertsController::setBonnetOpen(bool open)
{
    updateClosureState(m_bonnetOpen, open, &ClusterAlertsController::bonnetOpenChanged);
}

void ClusterAlertsController::setTrunkOpen(bool open)
{
    updateClosureState(m_trunkOpen, open, &ClusterAlertsController::trunkOpenChanged);
}

void ClusterAlertsController::setAllDoors(bool open)
{
    m_doorFrontLeft = open;
    m_doorFrontRight = open;
    m_doorRearLeft = open;
    m_doorRearRight = open;
    m_bonnetOpen = open;
    m_trunkOpen = open;
    m_doorOpenAlert = open;
    emit doorFrontLeftChanged();
    emit doorFrontRightChanged();
    emit doorRearLeftChanged();
    emit doorRearRightChanged();
    emit bonnetOpenChanged();
    emit trunkOpenChanged();
    emit doorOpenAlertChanged();
    if (open) emit playChimeRequested();
}

void ClusterAlertsController::setDoorOpenAlert(bool open)
{
    if (m_doorOpenAlert != open) {
        m_doorOpenAlert = open;
        if (!open) {
            setAllDoors(false);
        } else if (!isAnyDoorOpen()) {
            setDoorFrontRight(true); // Default driver door
        }
        emit doorOpenAlertChanged();
        if (open) emit warningChimeRequested();
    }
}

void ClusterAlertsController::setDoorOpenSide(int side)
{
    if (m_doorOpenSide != side) {
        m_doorOpenSide = side;
        if (side == 1) { // Driver (FR)
            m_doorFrontRight = true; m_doorFrontLeft = false; m_doorRearLeft = false; m_doorRearRight = false;
        } else if (side == 2) { // Passenger (FL)
            m_doorFrontLeft = true; m_doorFrontRight = false; m_doorRearLeft = false; m_doorRearRight = false;
        } else if (side == 3) { // Front Both
            m_doorFrontLeft = true; m_doorFrontRight = true; m_doorRearLeft = false; m_doorRearRight = false;
        } else if (side == 0) {
            setAllDoors(false);
        }
        m_doorOpenAlert = isAnyDoorOpen();
        emit doorFrontLeftChanged();
        emit doorFrontRightChanged();
        emit doorRearLeftChanged();
        emit doorRearRightChanged();
        emit doorOpenSideChanged();
        emit doorOpenAlertChanged();
        if (side > 0) emit playChimeRequested();
    }
}

void ClusterAlertsController::updateRearBuckled(bool &member, bool buckled, int seat, void (ClusterAlertsController::*changedSignal)())
{
    if (member != buckled) {
        member = buckled;
        emit (this->*changedSignal)();
        if (!buckled) {
            triggerRearAlarm(seat);
        } else if (m_rearAlarmSeat == seat) {
            stopRearAlarm();
        }
    }
}

void ClusterAlertsController::setRearLeftBuckled(bool buckled)
{
    updateRearBuckled(m_rearLeftBuckled, buckled, 0, &ClusterAlertsController::rearLeftBuckledChanged);
}

void ClusterAlertsController::setRearCenterBuckled(bool buckled)
{
    updateRearBuckled(m_rearCenterBuckled, buckled, 1, &ClusterAlertsController::rearCenterBuckledChanged);
}

void ClusterAlertsController::setRearRightBuckled(bool buckled)
{
    updateRearBuckled(m_rearRightBuckled, buckled, 2, &ClusterAlertsController::rearRightBuckledChanged);
}

void ClusterAlertsController::triggerRearAlarm(int seat)
{
    m_rearAlarmSeat = seat;
    emit rearAlarmSeatChanged();
    m_rearAlarmActive = true;
    emit rearAlarmActiveChanged();

    m_rearSeatBlinkState = true;
    emit rearSeatBlinkStateChanged();
    m_rearBlinkTimer.start();
    m_rearAlarmTimer.start(10000);

    emit seatbeltChimeRequested();
}

void ClusterAlertsController::stopRearAlarm()
{
    m_rearAlarmActive = false;
    emit rearAlarmActiveChanged();
    m_rearSeatBlinkState = false;
    emit rearSeatBlinkStateChanged();
    m_rearBlinkTimer.stop();
    m_rearAlarmTimer.stop();
}

void ClusterAlertsController::onRearBlinkTick()
{
    m_rearSeatBlinkState = !m_rearSeatBlinkState;
    emit rearSeatBlinkStateChanged();
}

void ClusterAlertsController::onRearAlarmTimeout()
{
    stopRearAlarm();
}

void ClusterAlertsController::setReduceSpeedAlert(bool active)
{
    if (m_reduceSpeedAlert != active) {
        m_reduceSpeedAlert = active;
        emit reduceSpeedAlertChanged();
        if (active) emit playChimeRequested();
    }
}

void ClusterAlertsController::triggerReduceSpeedAlert()
{
    setReduceSpeedAlert(true);
    m_reduceSpeedDismissTimer.start(4000);
}

void ClusterAlertsController::onReduceSpeedDismissTimeout()
{
    setReduceSpeedAlert(false);
}

void ClusterAlertsController::evaluateSpeed(int prevSpeed, int currentSpeed)
{
    if (currentSpeed >= 80 && prevSpeed < 80 && !m_speed80Triggered) {
        m_speed80Triggered = true;
        triggerReduceSpeedAlert();
        emit speedAlertChimeRequested();
    } else if (currentSpeed < 76) {
        m_speed80Triggered = false;
    }

    if (currentSpeed >= 120) {
        if (prevSpeed < 120 && !m_speed120Triggered) {
            m_speed120Triggered = true;
            triggerReduceSpeedAlert();
            emit speedAlertChimeRequested();
        }
        if (!m_overspeedBeepTimer.isActive()) {
            m_overspeedBeepTimer.start(1000);
            emit speedAlertChimeRequested();
        }
    } else {
        m_speed120Triggered = false;
        if (m_overspeedBeepTimer.isActive()) {
            m_overspeedBeepTimer.stop();
        }
    }
}

void ClusterAlertsController::onOverspeedBeepTick()
{
    emit speedAlertChimeRequested();
}

void ClusterAlertsController::setDriverAttentionActive(bool active)
{
    if (m_driverAttentionActive != active) {
        m_driverAttentionActive = active;
        emit driverAttentionActiveChanged();
        if (active) emit playChimeRequested();
    }
}

void ClusterAlertsController::setAttentionLevel(int level)
{
    if (m_attentionLevel != level) {
        m_attentionLevel = std::clamp(level, 1, 5);
        emit attentionLevelChanged();
    }
}

void ClusterAlertsController::triggerDriverAttention()
{
    setAttentionLevel(1);
    setDriverAttentionActive(true);
}

void ClusterAlertsController::setServiceDueKm(int km)
{
    if (m_serviceDueKm != km) {
        m_serviceDueKm = km;
        emit serviceDueKmChanged();
    }
}

void ClusterAlertsController::setServiceDueDays(int days)
{
    if (m_serviceDueDays != days) {
        m_serviceDueDays = days;
        emit serviceDueDaysChanged();
    }
}

void ClusterAlertsController::setServicePopupActive(bool active)
{
    if (m_servicePopupActive != active) {
        m_servicePopupActive = active;
        emit servicePopupActiveChanged();
        if (active) emit playChimeRequested();
    }
}

void ClusterAlertsController::triggerServiceReminder()
{
    setServicePopupActive(true);
}

void ClusterAlertsController::resetServiceInterval()
{
    setServiceDueKm(10000);
    setServiceDueDays(365);
    setServicePopupActive(false);
}

void ClusterAlertsController::setSunroofOpen(bool open)
{
    if (m_sunroofOpen != open) {
        m_sunroofOpen = open;
        emit sunroofOpenChanged();
    }
}

void ClusterAlertsController::setSunroofAlertActive(bool active)
{
    if (m_sunroofAlertActive != active) {
        m_sunroofAlertActive = active;
        emit sunroofAlertActiveChanged();
        if (active) emit playChimeRequested();
    }
}

void ClusterAlertsController::setSmartKeyPrompt(int prompt)
{
    if (m_smartKeyPrompt != prompt) {
        m_smartKeyPrompt = prompt;
        emit smartKeyPromptChanged();
        if (prompt > 0) emit playChimeRequested();
    }
}

void ClusterAlertsController::showSmartKeyAlert(int type)
{
    setSmartKeyPrompt(type);
}

void ClusterAlertsController::setIsgActive(bool active)
{
    if (m_isgActive != active) {
        m_isgActive = active;
        emit isgActiveChanged();
    }
}

void ClusterAlertsController::toggleIsg()
{
    setIsgActive(!m_isgActive);
}

void ClusterAlertsController::setPressStartAgainAlert(bool active)
{
    if (m_pressStartAgainAlert != active) {
        m_pressStartAgainAlert = active;
        emit pressStartAgainAlertChanged();
        if (active) emit keyAlertChimeRequested();
    }
}

void ClusterAlertsController::setStartPedalPrompt(int prompt)
{
    if (m_startPedalPrompt != prompt) {
        m_startPedalPrompt = prompt;
        emit startPedalPromptChanged();
        if (prompt > 0) emit keyAlertChimeRequested();
    }
}

void ClusterAlertsController::setCruiseEnabled(bool enabled)
{
    if (m_cruiseEnabled != enabled) {
        m_cruiseEnabled = enabled;
        if (!m_cruiseEnabled) {
            m_cruiseActive = false;
            emit cruiseActiveChanged();
        }
        emit cruiseEnabledChanged();
    }
}

void ClusterAlertsController::setCruiseActive(bool active)
{
    if (m_cruiseActive != active) {
        m_cruiseActive = active;
        if (m_cruiseActive && !m_cruiseEnabled) {
            m_cruiseEnabled = true;
            emit cruiseEnabledChanged();
        }
        emit cruiseActiveChanged();
    }
}

void ClusterAlertsController::setCruiseSetSpeed(int speed)
{
    if (m_cruiseSetSpeed != speed) {
        m_cruiseSetSpeed = speed;
        emit cruiseSetSpeedChanged();
    }
}

void ClusterAlertsController::toggleCruise()
{
    if (!m_cruiseEnabled) {
        setCruiseEnabled(true);
        setCruiseActive(false);
    } else {
        setCruiseEnabled(false);
        setCruiseActive(false);
    }
}

int ClusterAlertsController::applyCruiseSet(int currentSpeed)
{
    if (!m_cruiseEnabled) {
        setCruiseEnabled(true);
    }
    int target = currentSpeed > 30 ? currentSpeed : (m_cruiseSetSpeed > 0 ? m_cruiseSetSpeed : 41);
    setCruiseSetSpeed(target);
    setCruiseActive(true);
    return target;
}

int ClusterAlertsController::applyCruiseResPlus()
{
    if (!m_cruiseEnabled) {
        setCruiseEnabled(true);
    }
    setCruiseActive(true);
    int target = std::min(180, m_cruiseSetSpeed + 2);
    setCruiseSetSpeed(target);
    return target;
}

int ClusterAlertsController::applyCruiseSetMinus(int currentSpeed)
{
    if (!m_cruiseEnabled) {
        setCruiseEnabled(true);
    }
    if (!m_cruiseActive) {
        return applyCruiseSet(currentSpeed);
    }
    int target = std::max(30, m_cruiseSetSpeed - 2);
    setCruiseSetSpeed(target);
    return target;
}

void ClusterAlertsController::cruiseCancel()
{
    setCruiseActive(false);
}
