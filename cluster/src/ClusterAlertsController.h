/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterAlertsController.h
 * Author:         SK Rehan Ahamed
 * Description:    Alerts, Closures, Seatbelts, and Cruise Controller Header
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#pragma once
#include <QObject>
#include <QTimer>

class ClusterAlertsController : public QObject
{
    Q_OBJECT

public:
    explicit ClusterAlertsController(QObject *parent = nullptr);

    // Body Closures
    bool doorFrontLeft() const { return m_doorFrontLeft; }
    bool doorFrontRight() const { return m_doorFrontRight; }
    bool doorRearLeft() const { return m_doorRearLeft; }
    bool doorRearRight() const { return m_doorRearRight; }
    bool bonnetOpen() const { return m_bonnetOpen; }
    bool trunkOpen() const { return m_trunkOpen; }
    bool doorOpenAlert() const { return m_doorOpenAlert || isAnyDoorOpen(); }
    int doorOpenSide() const { return m_doorOpenSide; }
    bool isAnyDoorOpen() const
    {
        return m_doorFrontLeft || m_doorFrontRight || m_doorRearLeft || m_doorRearRight || m_bonnetOpen || m_trunkOpen;
    }

    void setDoorFrontLeft(bool open);
    void setDoorFrontRight(bool open);
    void setDoorRearLeft(bool open);
    void setDoorRearRight(bool open);
    void setBonnetOpen(bool open);
    void setTrunkOpen(bool open);
    void setAllDoors(bool open);
    void setDoorOpenAlert(bool open);
    void setDoorOpenSide(int side);

    // Rear Seatbelts
    bool rearLeftBuckled() const { return m_rearLeftBuckled; }
    bool rearCenterBuckled() const { return m_rearCenterBuckled; }
    bool rearRightBuckled() const { return m_rearRightBuckled; }
    bool rearAlarmActive() const { return m_rearAlarmActive; }
    int rearAlarmSeat() const { return m_rearAlarmSeat; }
    bool rearSeatBlinkState() const { return m_rearSeatBlinkState; }

    void setRearLeftBuckled(bool buckled);
    void setRearCenterBuckled(bool buckled);
    void setRearRightBuckled(bool buckled);
    void triggerRearAlarm(int seat);
    void stopRearAlarm();

    // Speed Alerts
    bool reduceSpeedAlert() const { return m_reduceSpeedAlert; }
    void setReduceSpeedAlert(bool active);
    void triggerReduceSpeedAlert();
    void evaluateSpeed(int prevSpeed, int currentSpeed);

    // Driver Attention & Prompts
    bool driverAttentionActive() const { return m_driverAttentionActive; }
    int attentionLevel() const { return m_attentionLevel; }
    int serviceDueKm() const { return m_serviceDueKm; }
    int serviceDueDays() const { return m_serviceDueDays; }
    bool servicePopupActive() const { return m_servicePopupActive; }
    bool sunroofOpen() const { return m_sunroofOpen; }
    bool sunroofAlertActive() const { return m_sunroofAlertActive; }
    int smartKeyPrompt() const { return m_smartKeyPrompt; }
    bool isgActive() const { return m_isgActive; }
    bool pressStartAgainAlert() const { return m_pressStartAgainAlert; }
    int startPedalPrompt() const { return m_startPedalPrompt; }

    void setDriverAttentionActive(bool active);
    void setAttentionLevel(int level);
    void triggerDriverAttention();
    void setServiceDueKm(int km);
    void setServiceDueDays(int days);
    void setServicePopupActive(bool active);
    void triggerServiceReminder();
    void resetServiceInterval();
    void setSunroofOpen(bool open);
    void setSunroofAlertActive(bool active);
    void setSmartKeyPrompt(int prompt);
    void showSmartKeyAlert(int type);
    void setIsgActive(bool active);
    void toggleIsg();
    void setPressStartAgainAlert(bool active);
    void setStartPedalPrompt(int prompt);

    // Cruise Control State
    bool cruiseEnabled() const { return m_cruiseEnabled; }
    bool cruiseActive() const { return m_cruiseActive; }
    int cruiseSetSpeed() const { return m_cruiseSetSpeed; }

    void setCruiseEnabled(bool enabled);
    void setCruiseActive(bool active);
    void setCruiseSetSpeed(int speed);
    void toggleCruise();
    int applyCruiseSet(int currentSpeed);
    int applyCruiseResPlus();
    int applyCruiseSetMinus(int currentSpeed);
    void cruiseCancel();

signals:
    void doorFrontLeftChanged();
    void doorFrontRightChanged();
    void doorRearLeftChanged();
    void doorRearRightChanged();
    void bonnetOpenChanged();
    void trunkOpenChanged();
    void doorOpenAlertChanged();
    void doorOpenSideChanged();

    void rearLeftBuckledChanged();
    void rearCenterBuckledChanged();
    void rearRightBuckledChanged();
    void rearAlarmActiveChanged();
    void rearAlarmSeatChanged();
    void rearSeatBlinkStateChanged();

    void reduceSpeedAlertChanged();
    void driverAttentionActiveChanged();
    void attentionLevelChanged();
    void serviceDueKmChanged();
    void serviceDueDaysChanged();
    void servicePopupActiveChanged();
    void sunroofOpenChanged();
    void sunroofAlertActiveChanged();
    void smartKeyPromptChanged();
    void isgActiveChanged();
    void pressStartAgainAlertChanged();
    void startPedalPromptChanged();

    void cruiseEnabledChanged();
    void cruiseActiveChanged();
    void cruiseSetSpeedChanged();

    void playChimeRequested();
    void warningChimeRequested();
    void seatbeltChimeRequested();
    void keyAlertChimeRequested();
    void speedAlertChimeRequested();

private slots:
    void onRearAlarmTimeout();
    void onRearBlinkTick();
    void onReduceSpeedDismissTimeout();
    void onOverspeedBeepTick();

private:
    void updateClosureState(bool &member, bool open, void (ClusterAlertsController::*changedSignal)());
    void updateRearBuckled(bool &member, bool buckled, int seat, void (ClusterAlertsController::*changedSignal)());

    bool m_doorFrontLeft = false;
    bool m_doorFrontRight = false;
    bool m_doorRearLeft = false;
    bool m_doorRearRight = false;
    bool m_bonnetOpen = false;
    bool m_trunkOpen = false;
    bool m_doorOpenAlert = false;
    int m_doorOpenSide = 0;

    bool m_rearLeftBuckled = true;
    bool m_rearCenterBuckled = true;
    bool m_rearRightBuckled = true;
    bool m_rearAlarmActive = false;
    int m_rearAlarmSeat = -1;
    bool m_rearSeatBlinkState = false;
    QTimer m_rearAlarmTimer;
    QTimer m_rearBlinkTimer;

    bool m_reduceSpeedAlert = false;
    bool m_speed80Triggered = false;
    bool m_speed120Triggered = false;
    QTimer m_reduceSpeedDismissTimer;
    QTimer m_overspeedBeepTimer;

    bool m_driverAttentionActive = false;
    int m_attentionLevel = 4;
    int m_serviceDueKm = 1500;
    int m_serviceDueDays = 30;
    bool m_servicePopupActive = false;
    bool m_sunroofOpen = false;
    bool m_sunroofAlertActive = false;
    int m_smartKeyPrompt = 0;
    bool m_isgActive = false;
    bool m_pressStartAgainAlert = false;
    int m_startPedalPrompt = 0;

    bool m_cruiseEnabled = false;
    bool m_cruiseActive = false;
    int m_cruiseSetSpeed = 41;
};
