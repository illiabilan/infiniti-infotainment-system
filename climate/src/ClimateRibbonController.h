#pragma once

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QString>
#include <algorithm>

class VehicleState;

class ClimateRibbonController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double driverTemp READ driverTemp WRITE setDriverTemp NOTIFY driverTempChanged)
    Q_PROPERTY(double passengerTemp READ passengerTemp WRITE setPassengerTemp NOTIFY passengerTempChanged)
    Q_PROPERTY(bool dualMode READ dualMode WRITE setDualMode NOTIFY dualModeChanged)
    Q_PROPERTY(int fanSpeed READ fanSpeed WRITE setFanSpeed NOTIFY fanSpeedChanged)
    Q_PROPERTY(int airflowMode READ airflowMode WRITE setAirflowMode NOTIFY airflowModeChanged)
    Q_PROPERTY(bool acActive READ acActive WRITE setAcActive NOTIFY acActiveChanged)
    Q_PROPERTY(bool autoActive READ autoActive WRITE setAutoActive NOTIFY autoActiveChanged)
    Q_PROPERTY(bool recircActive READ recircActive WRITE setRecircActive NOTIFY recircActiveChanged)
    Q_PROPERTY(bool ecoActive READ ecoActive WRITE setEcoActive NOTIFY ecoActiveChanged)
    Q_PROPERTY(bool frontDefrostActive READ frontDefrostActive WRITE setFrontDefrostActive NOTIFY frontDefrostActiveChanged)
    Q_PROPERTY(bool rearDefrostActive READ rearDefrostActive WRITE setRearDefrostActive NOTIFY rearDefrostActiveChanged)
    Q_PROPERTY(int driverSeatHeat READ driverSeatHeat WRITE setDriverSeatHeat NOTIFY driverSeatHeatChanged)
    Q_PROPERTY(int passengerSeatHeat READ passengerSeatHeat WRITE setPassengerSeatHeat NOTIFY passengerSeatHeatChanged)
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY clockChanged)
    Q_PROPERTY(QString currentDate READ currentDate NOTIFY clockChanged)
    Q_PROPERTY(int outsideTemp READ outsideTemp WRITE setOutsideTemp NOTIFY outsideTempChanged)

public:
    explicit ClimateRibbonController(QObject *parent = nullptr);
    void attachVehicleState(VehicleState *state);

    double driverTemp() const { return m_driverTemp; }
    double passengerTemp() const { return m_passengerTemp; }
    bool dualMode() const { return m_dualMode; }
    bool ecoActive() const { return m_ecoActive; }
    int fanSpeed() const { return m_fanSpeed; }
    int airflowMode() const { return m_airflowMode; }
    bool acActive() const { return m_acActive; }
    bool autoActive() const { return m_autoActive; }
    bool recircActive() const { return m_recircActive; }
    bool frontDefrostActive() const { return m_frontDefrostActive; }
    bool rearDefrostActive() const { return m_rearDefrostActive; }
    int driverSeatHeat() const { return m_driverSeatHeat; }
    int passengerSeatHeat() const { return m_passengerSeatHeat; }
    QString currentTime() const { return m_currentTime; }
    QString currentDate() const { return m_currentDate; }
    int outsideTemp() const { return m_outsideTemp; }

    void setDriverTemp(double temp);
    void setPassengerTemp(double temp);
    void setDualMode(bool enabled);
    void setEcoActive(bool enabled);
    void setFanSpeed(int speed);
    void setAirflowMode(int mode);
    void setAcActive(bool enabled);
    void setAutoActive(bool enabled);
    void setRecircActive(bool enabled);
    void setFrontDefrostActive(bool enabled);
    void setRearDefrostActive(bool enabled);
    void setDriverSeatHeat(int level);
    void setPassengerSeatHeat(int level);
    void setOutsideTemp(int temp);

    Q_INVOKABLE void stepDriverTemp(double delta);
    Q_INVOKABLE void stepPassengerTemp(double delta);
    Q_INVOKABLE void stepFanSpeed(int delta);
    Q_INVOKABLE void toggleAc();
    Q_INVOKABLE void toggleAuto();
    Q_INVOKABLE void toggleDual();
    Q_INVOKABLE void toggleRecirc();
    Q_INVOKABLE void toggleEco();
    Q_INVOKABLE void toggleFrontDefrost();
    Q_INVOKABLE void toggleRearDefrost();
    Q_INVOKABLE void cycleDriverSeatHeat();
    Q_INVOKABLE void cyclePassengerSeatHeat();

signals:
    void driverTempChanged();
    void passengerTempChanged();
    void dualModeChanged();
    void ecoActiveChanged();
    void fanSpeedChanged();
    void airflowModeChanged();
    void acActiveChanged();
    void autoActiveChanged();
    void recircActiveChanged();
    void frontDefrostActiveChanged();
    void rearDefrostActiveChanged();
    void driverSeatHeatChanged();
    void passengerSeatHeatChanged();
    void clockChanged();
    void outsideTempChanged();

private slots:
    void updateClock();

private:
    static double clampTemp(double temp);

    double m_driverTemp = 21.5;
    double m_passengerTemp = 21.5;
    bool m_dualMode = false;
    bool m_ecoActive = false;
    int m_fanSpeed = 3;
    int m_airflowMode = 0; // 0: Face, 1: Face+Feet, 2: Feet, 3: Defrost+Feet
    bool m_acActive = true;
    bool m_autoActive = true;
    bool m_recircActive = false;
    bool m_frontDefrostActive = false;
    bool m_rearDefrostActive = false;
    int m_driverSeatHeat = 0;
    int m_passengerSeatHeat = 0;
    int m_outsideTemp = 20;

    QString m_currentTime;
    QString m_currentDate;
    QTimer m_clockTimer;

    VehicleState *m_state = nullptr;
    bool m_syncingFromState = false;
    void syncToVehicleState();
};
