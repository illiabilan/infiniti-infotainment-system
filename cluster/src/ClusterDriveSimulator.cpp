/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterDriveSimulator.cpp
 * Author:         SK Rehan Ahamed
 * Description:    Cluster Drive Telemetry Simulator Implementation
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#include "ClusterDriveSimulator.h"
#include "ClusterController.h"
#include "ClusterTripComputer.h"
#include <algorithm>
#include <cmath>

ClusterDriveSimulator::ClusterDriveSimulator(QObject *parent)
    : QObject(parent)
{
    connect(&m_driveSimTimer, &QTimer::timeout, this, &ClusterDriveSimulator::tickRequested);
    m_driveSimTimer.start(33);
}

void ClusterDriveSimulator::toggleDemo(ClusterController &controller)
{
    m_isDemoDriving = !m_isDemoDriving;
    emit isDemoDrivingChanged();

    if (m_isDemoDriving) {
        if (controller.clusterState() != ClusterController::StateNormalTrip) {
            controller.triggerStartupSequence();
        }
        controller.setGear(QString::fromUtf8("D"));
        controller.setParkBrakeActive(false);
        controller.setBatteryActive(false);
        controller.setOilActive(false);
        controller.setSeatbeltActive(false);
        m_demoScenario = QString::fromUtf8("Starting Autonomous Drive");
        emit demoScenarioChanged();
    } else {
        m_demoCycleTime = 0.0;
        m_demoPrevGear = 1;
        controller.setGear(QString::fromUtf8("N"));
        controller.setSpeed(0);
        controller.setRpm(0.0);
        controller.setLeftIndicator(false);
        controller.setRightIndicator(false);
        controller.setCruiseActive(false);
        controller.setParkBrakeActive(true);
        controller.setBatteryActive(true);
        controller.setOilActive(true);
        controller.setSeatbeltActive(true);
        m_demoScenario = QString::fromUtf8("Idle / Parked");
        emit demoScenarioChanged();
    }
}

void ClusterDriveSimulator::applyManualSpeedAmtCoupling(ClusterController &controller, int speed)
{
    int currentGearNum = 1;
    if (speed <= 18) currentGearNum = 1;
    else if (speed <= 36) currentGearNum = 2;
    else if (speed <= 58) currentGearNum = 3;
    else if (speed <= 82) currentGearNum = 4;
    else currentGearNum = 5;

    if (speed == 0) {
        controller.setGear(QString::fromUtf8("D"));
    } else {
        controller.setGear(QString("D%1").arg(currentGearNum));
    }

    if (controller.clusterState() == ClusterController::StateNormalTrip) {
        if (speed == 0) {
            controller.setRpm(0.8);
        } else {
            double gearMin = (currentGearNum == 1) ? 0.0 : (currentGearNum == 2 ? 18.0 : (currentGearNum == 3 ? 36.0 : (currentGearNum == 4 ? 58.0 : 82.0)));
            double gearMax = (currentGearNum == 1) ? 24.0 : (currentGearNum == 2 ? 44.0 : (currentGearNum == 3 ? 68.0 : (currentGearNum == 4 ? 92.0 : 175.0)));
            double frac = (speed - gearMin) / std::max(1.0, gearMax - gearMin);
            frac = std::clamp(frac, 0.0, 1.0);

            double baseR = (currentGearNum == 1) ? 0.9 : 1.4;
            double topR = (currentGearNum == 5) ? 5.8 : 3.2;
            double calcRpm = baseR + frac * (topR - baseR);
            controller.setRpm(std::round(calcRpm * 10.0) / 10.0);
        }
    }
}

void ClusterDriveSimulator::onSimulationTick(ClusterController &controller, ClusterTripComputer &trip, bool hasVehicleState)
{
    if (!hasVehicleState && m_isDemoDriving && controller.clusterState() == ClusterController::StateNormalTrip) {
        m_demoCycleTime += 0.033; // 30Hz tick

        double phase = std::fmod(m_demoCycleTime, 60.0);
        double targetSpeed = 0.0;
        QString scenario = QString::fromUtf8("City Driving");

        if (phase < 8.0) {
            double p = phase / 8.0;
            targetSpeed = 45.0 * std::sin(p * 1.5708);
            scenario = QString("City Acceleration (%1 km/h)").arg(controller.speed());
            controller.setCruiseActive(false);
            controller.setLeftIndicator(false);
            controller.setRightIndicator(false);
        } else if (phase < 13.0) {
            double p = (phase - 8.0) / 5.0;
            targetSpeed = 45.0 + 10.0 * p;
            scenario = QString::fromUtf8("Left Lane Change (Indicator Active)");
            controller.setLeftIndicator(true);
            controller.setRightIndicator(false);
        } else if (phase < 24.0) {
            controller.setLeftIndicator(false);
            double p = (phase - 13.0) / 11.0;
            targetSpeed = 55.0 + 50.0 * std::sin(p * 1.5708);
            scenario = QString("Highway Acceleration (%1 km/h)").arg(controller.speed());
        } else if (phase < 38.0) {
            targetSpeed = 100.0 + 2.0 * std::sin((phase - 24.0) * 0.8);
            scenario = QString::fromUtf8("Highway Cruise Control (100 km/h Active)");
            controller.setCruiseEnabled(true);
            controller.setCruiseActive(true);
            controller.setCruiseSetSpeed(100);
        } else if (phase < 43.0) {
            controller.setCruiseActive(false);
            controller.setRightIndicator(true);
            double p = (phase - 38.0) / 5.0;
            targetSpeed = 100.0 - 20.0 * p;
            scenario = QString::fromUtf8("Right Lane Exit (Indicator Active)");
        } else if (phase < 54.0) {
            controller.setRightIndicator(false);
            double p = (phase - 43.0) / 11.0;
            targetSpeed = 80.0 * (1.0 - std::sin(p * 1.5708));
            if (targetSpeed < 0.0) targetSpeed = 0.0;
            scenario = QString("Decelerating to Stop (%1 km/h)").arg(controller.speed());
        } else {
            targetSpeed = 0.0;
            scenario = QString::fromUtf8("Traffic Light Stop (Idle)");
        }

        double currentSpeed = controller.speed() + (targetSpeed - controller.speed()) * 0.08;
        int nextSpeed = static_cast<int>(std::round(currentSpeed));
        if (nextSpeed < 0) nextSpeed = 0;
        controller.setSpeed(nextSpeed);

        int currentGearNum = 1;
        if (nextSpeed <= 16) currentGearNum = 1;
        else if (nextSpeed <= 34) currentGearNum = 2;
        else if (nextSpeed <= 58) currentGearNum = 3;
        else if (nextSpeed <= 82) currentGearNum = 4;
        else currentGearNum = 5;

        if (nextSpeed == 0) {
            controller.setGear(QString::fromUtf8("D"));
        } else {
            controller.setGear(QString("D%1").arg(currentGearNum));
        }

        double gearMinSpeed = (currentGearNum == 1) ? 0 : (currentGearNum == 2 ? 16 : (currentGearNum == 3 ? 34 : (currentGearNum == 4 ? 58 : 82)));
        double gearMaxSpeed = (currentGearNum == 1) ? 22 : (currentGearNum == 2 ? 42 : (currentGearNum == 3 ? 66 : (currentGearNum == 4 ? 90 : 165)));
        double fraction = (nextSpeed - gearMinSpeed) / std::max(1.0, gearMaxSpeed - gearMinSpeed);
        fraction = std::clamp(fraction, 0.0, 1.0);

        double calculatedRpm = 0.9;
        if (nextSpeed > 0) {
            calculatedRpm = 1.1 + fraction * 2.8;
            if (currentGearNum != m_demoPrevGear) {
                calculatedRpm = 1.2;
                m_demoPrevGear = currentGearNum;
            }
        }
        controller.setRpm(calculatedRpm);

        double instantEco = 18.0;
        if (nextSpeed == 0) {
            instantEco = 0.0;
        } else if (targetSpeed > currentSpeed + 2.0) {
            instantEco = 9.5 + 4.0 * (1.0 - fraction);
        } else if (targetSpeed < currentSpeed - 2.0) {
            instantEco = 28.5 + 1.5 * std::sin(m_demoCycleTime * 2.0);
        } else {
            instantEco = 19.2 + 2.5 * std::sin(m_demoCycleTime * 0.5);
        }
        trip.setInstantEconomy(std::clamp(instantEco, 0.0, 30.0));

        double distanceIncrement = (currentSpeed / 3600.0) * 0.033 * 5.0;
        auto delta = trip.advanceDistanceAndFuel(distanceIncrement, instantEco, 0.005, 0.1, true, controller.fuelLevel());
        if (delta.odoKmToAdd > 0) {
            controller.setOdoKm(controller.odoKm() + delta.odoKmToAdd);
        }
        if (delta.dteKmToSub > 0 && controller.dteKm() > delta.dteKmToSub) {
            controller.setDteKm(controller.dteKm() - delta.dteKmToSub);
        }
        if (delta.fuelBarsToSub > 0 && controller.fuelLevel() > 0) {
            controller.setFuelLevel(controller.fuelLevel() - delta.fuelBarsToSub);
        }

        if (m_demoScenario != scenario) {
            m_demoScenario = scenario;
            emit demoScenarioChanged();
        }
    }
    else if (!hasVehicleState && !m_isDemoDriving && controller.speed() > 0 && controller.clusterState() == ClusterController::StateNormalTrip) {
        double speedVal = static_cast<double>(controller.speed());
        double instantEco = 16.0;
        if (speedVal <= 15.0) instantEco = 8.5 + speedVal * 0.3;
        else if (speedVal <= 65.0) instantEco = 13.0 + (speedVal - 15.0) * 0.22;
        else if (speedVal <= 90.0) instantEco = 24.0 - (speedVal - 65.0) * 0.15;
        else instantEco = 20.2 - (speedVal - 90.0) * 0.12;
        trip.setInstantEconomy(std::clamp(instantEco, 0.0, 30.0));

        double distanceIncrement = (speedVal / 3600.0) * 0.033 * 12.0;
        auto delta = trip.advanceDistanceAndFuel(distanceIncrement, instantEco, 0.002, 0.05, false, controller.fuelLevel());
        if (delta.odoKmToAdd > 0) {
            controller.setOdoKm(controller.odoKm() + delta.odoKmToAdd);
        }
        if (delta.dteKmToSub > 0 && controller.dteKm() > delta.dteKmToSub) {
            controller.setDteKm(controller.dteKm() - delta.dteKmToSub);
        }
        if (delta.fuelBarsToSub > 0 && controller.fuelLevel() > 0) {
            controller.setFuelLevel(controller.fuelLevel() - delta.fuelBarsToSub);
        }
    }

    if (controller.clusterState() == ClusterController::StateNormalTrip && controller.speed() > 0) {
        trip.advanceEngineRuntime(0.20);
    }
}
