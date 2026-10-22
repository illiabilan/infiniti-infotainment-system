/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterTripComputer.h
 * Author:         SK Rehan Ahamed
 * Description:    Trip Computer and Fuel Economy Calculator Header
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#pragma once
#include <QObject>
#include <QString>

class ClusterTripComputer : public QObject
{
    Q_OBJECT

public:
    struct DistanceTickDelta {
        int odoKmToAdd = 0;
        int dteKmToSub = 0;
        int fuelBarsToSub = 0;
    };

    explicit ClusterTripComputer(QObject *parent = nullptr);

    double tripKm() const { return m_tripKm; }
    QString tripTime() const { return m_tripTime; }
    double tripEconomy() const { return m_tripEconomy; }
    int tripPage() const { return m_tripPage; }
    double refuelKm() const { return m_refuelKm; }
    QString refuelTime() const { return m_refuelTime; }
    double refuelEconomy() const { return m_refuelEconomy; }
    double accumKm() const { return m_accumKm; }
    QString accumTime() const { return m_accumTime; }
    double accumEconomy() const { return m_accumEconomy; }
    double instantEconomy() const { return m_instantEconomy; }

    void setTripKm(double km);
    void setTripTime(const QString &time);
    void setTripEconomy(double econ);
    void setTripPage(int page);
    void nextTripPage();
    void prevTripPage();

    void setRefuelKm(double km);
    void setRefuelTime(const QString &time);
    void setRefuelEconomy(double econ);

    void setAccumKm(double km);
    void setAccumTime(const QString &time);
    void setAccumEconomy(double econ);

    void setInstantEconomy(double econ);

    void resetTrip();
    void resetSinceRefuel();
    void resetAccumInfo();
    void resetActiveTripPage();

    int calculateDteForFuelLevel(int fuelLevel) const;
    DistanceTickDelta advanceDistanceAndFuel(double distanceIncrement,
                                            double instantEco,
                                            double minLitresForAvg,
                                            double minKmForAvg,
                                            bool resetEconomyBelowMin,
                                            int currentFuelLevel);
    void advanceEngineRuntime(double timeRateSeconds);

signals:
    void tripKmChanged();
    void tripTimeChanged();
    void tripEconomyChanged();
    void tripPageChanged();
    void refuelKmChanged();
    void refuelTimeChanged();
    void refuelEconomyChanged();
    void accumKmChanged();
    void accumTimeChanged();
    void accumEconomyChanged();
    void instantEconomyChanged();

private:
    double m_tripKm = 0.0;
    QString m_tripTime = "0:00";
    double m_tripEconomy = 0.0;
    int m_tripSeconds = 0;
    int m_tripPage = 1; // 0 = Drive info, 1 = Since refuelling, 2 = Accumulated info
    double m_refuelKm = 0.0;
    QString m_refuelTime = "0:00";
    double m_refuelEconomy = 0.0;
    int m_refuelSeconds = 0;
    double m_accumKm = 0.0;
    QString m_accumTime = "0:00";
    double m_accumEconomy = 0.0;
    int m_accumSeconds = 0;
    double m_instantEconomy = 0.0;
    double m_tripLitres = 0.0;
    double m_refuelLitres = 0.0;
    double m_accumLitres = 0.0;
    double m_fuelBarAccumulator = 0.0;
    double m_engineSecAcc = 0.0;
    double m_rawTripKm = 0.0;
    double m_rawRefuelKm = 0.0;
    double m_rawAccumKm = 0.0;
    double m_rawOdoAcc = 0.0;
    double m_rawDteAcc = 0.0;
};
