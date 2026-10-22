/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterTripComputer.cpp
 * Author:         SK Rehan Ahamed
 * Description:    Trip Computer and Fuel Economy Calculator Implementation
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#include "ClusterTripComputer.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

ClusterTripComputer::ClusterTripComputer(QObject *parent)
    : QObject(parent)
{
}

void ClusterTripComputer::setTripKm(double km)
{
    m_tripKm = km;
    emit tripKmChanged();
}

void ClusterTripComputer::setTripTime(const QString &time)
{
    if (m_tripTime != time) {
        m_tripTime = time;
        emit tripTimeChanged();
    }
}

void ClusterTripComputer::setTripEconomy(double econ)
{
    m_tripEconomy = econ;
    emit tripEconomyChanged();
}

void ClusterTripComputer::setTripPage(int page)
{
    int p = (page % 3 + 3) % 3;
    if (m_tripPage != p) {
        m_tripPage = p;
        emit tripPageChanged();
    }
}

void ClusterTripComputer::nextTripPage()
{
    setTripPage(m_tripPage + 1);
}

void ClusterTripComputer::prevTripPage()
{
    setTripPage(m_tripPage - 1);
}

void ClusterTripComputer::setRefuelKm(double km)
{
    m_refuelKm = km;
    emit refuelKmChanged();
}

void ClusterTripComputer::setRefuelTime(const QString &time)
{
    if (m_refuelTime != time) {
        m_refuelTime = time;
        emit refuelTimeChanged();
    }
}

void ClusterTripComputer::setRefuelEconomy(double econ)
{
    m_refuelEconomy = econ;
    emit refuelEconomyChanged();
}

void ClusterTripComputer::setAccumKm(double km)
{
    m_accumKm = km;
    emit accumKmChanged();
}

void ClusterTripComputer::setAccumTime(const QString &time)
{
    if (m_accumTime != time) {
        m_accumTime = time;
        emit accumTimeChanged();
    }
}

void ClusterTripComputer::setAccumEconomy(double econ)
{
    m_accumEconomy = econ;
    emit accumEconomyChanged();
}

void ClusterTripComputer::setInstantEconomy(double econ)
{
    if (std::abs(m_instantEconomy - econ) > 0.01) {
        m_instantEconomy = std::clamp(econ, 0.0, 30.0);
        emit instantEconomyChanged();
    }
}

void ClusterTripComputer::resetTrip()
{
    m_tripSeconds = 0;
    m_tripLitres = 0.0;
    m_rawTripKm = 0.0;
    m_engineSecAcc = 0.0;
    setTripKm(0.0);
    setTripTime(QString::fromUtf8("0:00"));
    setTripEconomy(0.0);
}

void ClusterTripComputer::resetSinceRefuel()
{
    m_refuelSeconds = 0;
    m_refuelLitres = 0.0;
    m_rawRefuelKm = 0.0;
    setRefuelKm(0.0);
    setRefuelTime(QString::fromUtf8("0:00"));
    setRefuelEconomy(0.0);
}

void ClusterTripComputer::resetAccumInfo()
{
    m_accumSeconds = 0;
    m_accumLitres = 0.0;
    m_rawAccumKm = 0.0;
    setAccumKm(0.0);
    setAccumTime(QString::fromUtf8("0:00"));
    setAccumEconomy(0.0);
}

void ClusterTripComputer::resetActiveTripPage()
{
    if (m_tripPage == 1) {
        resetSinceRefuel();
    } else if (m_tripPage == 2) {
        resetAccumInfo();
    } else {
        resetTrip();
    }
}

int ClusterTripComputer::calculateDteForFuelLevel(int fuelLevel) const
{
    double remainingLitres = (fuelLevel / 12.0) * 37.0;
    double effectiveEcon = m_refuelEconomy > 5.0 ? m_refuelEconomy
                                                 : (m_accumEconomy > 5.0 ? m_accumEconomy : 15.5);
    return static_cast<int>(std::round(remainingLitres * effectiveEcon));
}

ClusterTripComputer::DistanceTickDelta ClusterTripComputer::advanceDistanceAndFuel(
    double distanceIncrement,
    double instantEco,
    double minLitresForAvg,
    double minKmForAvg,
    bool resetEconomyBelowMin,
    int currentFuelLevel)
{
    DistanceTickDelta delta;

    m_rawTripKm += distanceIncrement;
    m_rawRefuelKm += distanceIncrement;
    m_rawAccumKm += distanceIncrement;
    setTripKm(std::round(m_rawTripKm * 10.0) / 10.0);
    setRefuelKm(std::round(m_rawRefuelKm * 10.0) / 10.0);
    setAccumKm(std::round(m_rawAccumKm * 10.0) / 10.0);

    m_rawOdoAcc += distanceIncrement;
    if (m_rawOdoAcc >= 1.0) {
        int kmToAdd = static_cast<int>(m_rawOdoAcc);
        delta.odoKmToAdd = kmToAdd;
        m_rawOdoAcc -= kmToAdd;
    }

    m_rawDteAcc += distanceIncrement;
    if (m_rawDteAcc >= 1.0) {
        int kmToSub = static_cast<int>(m_rawDteAcc);
        delta.dteKmToSub = kmToSub;
        m_rawDteAcc -= kmToSub;
    }

    double litresDelta = (instantEco > 1.0) ? (distanceIncrement / instantEco)
                                           : (distanceIncrement / 16.0);
    m_tripLitres += litresDelta;
    m_refuelLitres += litresDelta;
    m_accumLitres += litresDelta;
    m_fuelBarAccumulator += litresDelta;
    if (m_fuelBarAccumulator >= 3.08 && currentFuelLevel > 0) {
        m_fuelBarAccumulator -= 3.08;
        delta.fuelBarsToSub = 1;
    }

    auto updateEcon = [this](double km,
                             double litres,
                             double minL,
                             double minK,
                             bool resetBelow,
                             auto setter) {
        if (litres > minL && km >= minK) {
            double avg = km / litres;
            setter(std::clamp(std::round(avg * 10.0) / 10.0, 5.0, 30.0));
        } else if (resetBelow) {
            setter(0.0);
        }
    };

    updateEcon(m_tripKm,
               m_tripLitres,
               minLitresForAvg,
               minKmForAvg,
               resetEconomyBelowMin,
               [this](double e) { setTripEconomy(e); });
    updateEcon(m_refuelKm,
               m_refuelLitres,
               minLitresForAvg,
               minKmForAvg,
               resetEconomyBelowMin,
               [this](double e) { setRefuelEconomy(e); });
    updateEcon(m_accumKm,
               m_accumLitres,
               minLitresForAvg,
               minKmForAvg,
               resetEconomyBelowMin,
               [this](double e) { setAccumEconomy(e); });

    return delta;
}

void ClusterTripComputer::advanceEngineRuntime(double timeRateSeconds)
{
    m_engineSecAcc += timeRateSeconds;
    if (m_engineSecAcc >= 1.0) {
        int sec = static_cast<int>(m_engineSecAcc);
        m_engineSecAcc -= sec;
        m_tripSeconds += sec;
        m_refuelSeconds += sec;
        m_accumSeconds += sec;

        auto fmtTime = [](int s) -> QString {
            int mins = (s / 60) % 60;
            int hours = s / 3600;
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%d:%02d", hours, mins);
            return QString::fromUtf8(buf);
        };

        setTripTime(fmtTime(m_tripSeconds));
        setRefuelTime(fmtTime(m_refuelSeconds));
        setAccumTime(fmtTime(m_accumSeconds));
    }
}
