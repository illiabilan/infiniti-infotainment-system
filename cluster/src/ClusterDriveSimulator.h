/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterDriveSimulator.h
 * Author:         SK Rehan Ahamed
 * Description:    Cluster Drive Telemetry Simulator Header
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#pragma once
#include <QObject>
#include <QString>
#include <QTimer>

class ClusterController;
class ClusterTripComputer;

class ClusterDriveSimulator : public QObject
{
    Q_OBJECT

public:
    explicit ClusterDriveSimulator(QObject *parent = nullptr);

    bool isDemoDriving() const { return m_isDemoDriving; }
    QString demoScenario() const { return m_demoScenario; }

    void toggleDemo(ClusterController &controller);
    void applyManualSpeedAmtCoupling(ClusterController &controller, int speed);
    void onSimulationTick(ClusterController &controller, ClusterTripComputer &trip, bool hasVehicleState);

signals:
    void isDemoDrivingChanged();
    void demoScenarioChanged();
    void tickRequested();

private:
    QTimer m_driveSimTimer;
    bool m_isDemoDriving = false;
    double m_demoCycleTime = 0.0;
    int m_demoPrevGear = 1;
    QString m_demoScenario = "Idle / Parked";
};
