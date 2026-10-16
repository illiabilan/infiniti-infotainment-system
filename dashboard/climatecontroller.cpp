#include "climatecontroller.h"

#include <algorithm>
#include <QtMath>

ClimateController::ClimateController(QObject *parent)
    : QObject(parent)
{
}

void ClimateController::attachVehicleState(VehicleState *state)
{
    m_state = state;
    if (!m_state)
        return;

    connect(m_state, &VehicleState::driverTempChanged, this, [this]() {
        m_syncingFromState = true;
        setDriverTemp(m_state->driverTemp());
        m_syncingFromState = false;
    });
    connect(m_state, &VehicleState::passengerTempChanged, this, [this]() {
        m_syncingFromState = true;
        setPassengerTemp(m_state->passengerTemp());
        m_syncingFromState = false;
    });
    connect(m_state, &VehicleState::fanSpeedChanged, this, [this]() {
        m_syncingFromState = true;
        setFanSpeed(m_state->fanSpeed() / 7.0);
        m_syncingFromState = false;
    });
    connect(m_state, &VehicleState::acActiveChanged, this, [this]() {
        m_syncingFromState = true;
        setAcOn(m_state->acActive());
        m_syncingFromState = false;
    });
    connect(m_state, &VehicleState::autoActiveChanged, this, [this]() {
        m_syncingFromState = true;
        setAutoMode(m_state->autoActive());
        m_syncingFromState = false;
    });
    connect(m_state, &VehicleState::frontDefrostActiveChanged, this, [this]() {
        m_syncingFromState = true;
        setDefrostOn(m_state->frontDefrostActive());
        m_syncingFromState = false;
    });
    connect(m_state, &VehicleState::driverSeatHeatChanged, this, [this]() {
        m_syncingFromState = true;
        setSeatHeatOn(m_state->driverSeatHeat() > 0);
        m_syncingFromState = false;
    });

    m_syncingFromState = true;
    setDriverTemp(m_state->driverTemp());
    setPassengerTemp(m_state->passengerTemp());
    setFanSpeed(m_state->fanSpeed() / 7.0);
    setAcOn(m_state->acActive());
    setAutoMode(m_state->autoActive());
    setDefrostOn(m_state->frontDefrostActive());
    setSeatHeatOn(m_state->driverSeatHeat() > 0);
    m_syncingFromState = false;
}

qreal ClimateController::driverTemp() const { return m_driverTemp; }
qreal ClimateController::passengerTemp() const { return m_passengerTemp; }
qreal ClimateController::fanSpeed() const { return m_fanSpeed; }
bool ClimateController::autoMode() const { return m_autoMode; }
bool ClimateController::acOn() const { return m_acOn; }
bool ClimateController::defrostOn() const { return m_defrostOn; }
bool ClimateController::seatHeatOn() const { return m_seatHeatOn; }

void ClimateController::setDriverTemp(qreal temp)
{
    const qreal normalized = clampTemp(temp);
    if (qFuzzyCompare(m_driverTemp, normalized))
        return;
    m_driverTemp = normalized;
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setDriverTemp(m_driverTemp);
        m_state->broadcastClimate();
    }
}

void ClimateController::setPassengerTemp(qreal temp)
{
    const qreal normalized = clampTemp(temp);
    if (qFuzzyCompare(m_passengerTemp, normalized))
        return;
    m_passengerTemp = normalized;
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setPassengerTemp(m_passengerTemp);
        m_state->broadcastClimate();
    }
}

void ClimateController::setFanSpeed(qreal speed)
{
    const qreal normalized = clamp01(speed);
    if (qFuzzyCompare(m_fanSpeed, normalized))
        return;
    m_fanSpeed = normalized;
    if (normalized > 0.0 && !m_syncingFromState)
        m_autoMode = false;
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setFanSpeed(qRound(m_fanSpeed * 7.0));
        if (normalized > 0.0)
            m_state->setAutoActive(false);
        m_state->broadcastClimate();
    }
}

void ClimateController::setAutoMode(bool enabled)
{
    if (m_autoMode == enabled)
        return;
    m_autoMode = enabled;
    if (enabled && !m_syncingFromState)
        m_fanSpeed = 4.0 / 7.0;
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setAutoActive(m_autoMode);
        if (enabled)
            m_state->setFanSpeed(4);
        m_state->broadcastClimate();
    }
}

void ClimateController::setAcOn(bool enabled)
{
    if (m_acOn == enabled)
        return;
    m_acOn = enabled;
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setAcActive(m_acOn);
        m_state->broadcastClimate();
    }
}

void ClimateController::setDefrostOn(bool enabled)
{
    if (m_defrostOn == enabled)
        return;
    m_defrostOn = enabled;
    if (enabled && !m_syncingFromState) {
        m_acOn = true;
        m_fanSpeed = std::max(m_fanSpeed, 5.0 / 7.0);
    }
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setFrontDefrostActive(m_defrostOn);
        m_state->setRearDefrostActive(m_defrostOn);
        if (m_defrostOn) {
            m_state->setAcActive(true);
            m_state->setFanSpeed(std::max(m_state->fanSpeed(), 5));
        }
        m_state->broadcastClimate();
    }
}

void ClimateController::setSeatHeatOn(bool enabled)
{
    if (m_seatHeatOn == enabled)
        return;
    m_seatHeatOn = enabled;
    emit climateChanged();
    if (!m_syncingFromState && m_state) {
        m_state->setDriverSeatHeat(m_seatHeatOn ? 2 : 0);
        m_state->broadcastClimate();
    }
}

void ClimateController::stepDriverTemp(qreal delta)
{
    setDriverTemp(m_driverTemp + delta);
}

void ClimateController::stepPassengerTemp(qreal delta)
{
    setPassengerTemp(m_passengerTemp + delta);
}

void ClimateController::toggleAutoMode()
{
    setAutoMode(!m_autoMode);
}

void ClimateController::toggleAc()
{
    setAcOn(!m_acOn);
}

void ClimateController::toggleDefrost()
{
    setDefrostOn(!m_defrostOn);
}

void ClimateController::toggleSeatHeat()
{
    setSeatHeatOn(!m_seatHeatOn);
}

qreal ClimateController::clampTemp(qreal temp)
{
    return std::clamp(temp, 16.0, 30.0);
}

qreal ClimateController::clamp01(qreal value)
{
    return std::clamp(value, 0.0, 1.0);
}
