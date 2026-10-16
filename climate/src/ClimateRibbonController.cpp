#include "ClimateRibbonController.h"
#include "VehicleState.h"

ClimateRibbonController::ClimateRibbonController(QObject *parent)
    : QObject(parent)
{
    updateClock();
    connect(&m_clockTimer, &QTimer::timeout, this, &ClimateRibbonController::updateClock);
    m_clockTimer.start(1000);
}

double ClimateRibbonController::clampTemp(double temp)
{
    return std::clamp(temp, 16.0, 32.0);
}

void ClimateRibbonController::updateClock()
{
    const QTime nowTime = QTime::currentTime();
    const QDate nowDate = QDate::currentDate();
    const QString newTime = nowTime.toString(QStringLiteral("HH:mm"));
    const QString newDate = nowDate.toString(QStringLiteral("ddd, MMM d")).toUpper();

    if (m_currentTime != newTime || m_currentDate != newDate) {
        m_currentTime = newTime;
        m_currentDate = newDate;
        emit clockChanged();
    }
}

void ClimateRibbonController::setDriverTemp(double temp)
{
    const double clamped = clampTemp(temp);
    if (qFuzzyCompare(m_driverTemp, clamped))
        return;

    m_driverTemp = clamped;
    emit driverTempChanged();

    if (!m_dualMode) {
        m_passengerTemp = clamped;
        emit passengerTempChanged();
    }
    syncToVehicleState();
}

void ClimateRibbonController::setPassengerTemp(double temp)
{
    const double clamped = clampTemp(temp);
    if (qFuzzyCompare(m_passengerTemp, clamped))
        return;

    m_passengerTemp = clamped;
    emit passengerTempChanged();

    if (!m_dualMode && !qFuzzyCompare(m_driverTemp, m_passengerTemp)) {
        m_dualMode = true;
        emit dualModeChanged();
    }
    syncToVehicleState();
}

void ClimateRibbonController::setDualMode(bool enabled)
{
    if (m_dualMode == enabled)
        return;

    m_dualMode = enabled;
    emit dualModeChanged();

    if (!m_dualMode) {
        m_passengerTemp = m_driverTemp;
        emit passengerTempChanged();
    }
    syncToVehicleState();
}

void ClimateRibbonController::setFanSpeed(int speed)
{
    const int clamped = std::clamp(speed, 0, 7);
    if (m_fanSpeed == clamped)
        return;

    m_fanSpeed = clamped;
    emit fanSpeedChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setAirflowMode(int mode)
{
    const int clamped = std::clamp(mode, 0, 3);
    if (m_airflowMode == clamped)
        return;

    m_airflowMode = clamped;
    emit airflowModeChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setAcActive(bool enabled)
{
    if (m_acActive == enabled)
        return;

    m_acActive = enabled;
    emit acActiveChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setAutoActive(bool enabled)
{
    if (m_autoActive == enabled)
        return;

    m_autoActive = enabled;
    emit autoActiveChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setRecircActive(bool enabled)
{
    if (m_recircActive == enabled)
        return;

    m_recircActive = enabled;
    emit recircActiveChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setFrontDefrostActive(bool enabled)
{
    if (m_frontDefrostActive == enabled)
        return;

    m_frontDefrostActive = enabled;
    emit frontDefrostActiveChanged();

    if (m_frontDefrostActive) {
        m_acActive = true;
        emit acActiveChanged();
        if (m_fanSpeed < 5) {
            m_fanSpeed = 5;
            emit fanSpeedChanged();
        }
    }
    syncToVehicleState();
}

void ClimateRibbonController::setRearDefrostActive(bool enabled)
{
    if (m_rearDefrostActive == enabled)
        return;

    m_rearDefrostActive = enabled;
    emit rearDefrostActiveChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setDriverSeatHeat(int level)
{
    const int clamped = std::clamp(level, 0, 3);
    if (m_driverSeatHeat == clamped)
        return;

    m_driverSeatHeat = clamped;
    emit driverSeatHeatChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setPassengerSeatHeat(int level)
{
    const int clamped = std::clamp(level, 0, 3);
    if (m_passengerSeatHeat == clamped)
        return;

    m_passengerSeatHeat = clamped;
    emit passengerSeatHeatChanged();
    syncToVehicleState();
}

void ClimateRibbonController::setOutsideTemp(int temp)
{
    if (m_outsideTemp == temp)
        return;

    m_outsideTemp = temp;
    emit outsideTempChanged();
}

void ClimateRibbonController::stepDriverTemp(double delta)
{
    setDriverTemp(m_driverTemp + delta);
}

void ClimateRibbonController::stepPassengerTemp(double delta)
{
    setPassengerTemp(m_passengerTemp + delta);
}

void ClimateRibbonController::stepFanSpeed(int delta)
{
    setFanSpeed(m_fanSpeed + delta);
}

void ClimateRibbonController::toggleAc()
{
    setAcActive(!m_acActive);
}

void ClimateRibbonController::toggleAuto()
{
    setAutoActive(!m_autoActive);
}

void ClimateRibbonController::toggleDual()
{
    setDualMode(!m_dualMode);
}

void ClimateRibbonController::toggleRecirc()
{
    setRecircActive(!m_recircActive);
}

void ClimateRibbonController::toggleFrontDefrost()
{
    setFrontDefrostActive(!m_frontDefrostActive);
}

void ClimateRibbonController::toggleRearDefrost()
{
    setRearDefrostActive(!m_rearDefrostActive);
}

void ClimateRibbonController::cycleDriverSeatHeat()
{
    setDriverSeatHeat((m_driverSeatHeat + 1) % 4);
}

void ClimateRibbonController::cyclePassengerSeatHeat()
{
    setPassengerSeatHeat((m_passengerSeatHeat + 1) % 4);
}

void ClimateRibbonController::attachVehicleState(VehicleState *state)
{
    m_state = state;
    if (!m_state)
        return;

    m_syncingFromState = true;
    m_driverTemp = m_state->driverTemp();
    m_passengerTemp = m_state->passengerTemp();
    m_fanSpeed = m_state->fanSpeed();
    m_airflowMode = m_state->airflowMode();
    m_acActive = m_state->acActive();
    m_autoActive = m_state->autoActive();
    m_dualMode = m_state->dualMode();
    m_recircActive = m_state->recircActive();
    m_frontDefrostActive = m_state->frontDefrostActive();
    m_rearDefrostActive = m_state->rearDefrostActive();
    m_driverSeatHeat = m_state->driverSeatHeat();
    m_passengerSeatHeat = m_state->passengerSeatHeat();
    m_outsideTemp = m_state->ambientTemp();
    m_syncingFromState = false;

    emit driverTempChanged();
    emit passengerTempChanged();
    emit fanSpeedChanged();
    emit airflowModeChanged();
    emit acActiveChanged();
    emit autoActiveChanged();
    emit dualModeChanged();
    emit recircActiveChanged();
    emit frontDefrostActiveChanged();
    emit rearDefrostActiveChanged();
    emit driverSeatHeatChanged();
    emit passengerSeatHeatChanged();
    emit outsideTempChanged();

    connect(m_state, &VehicleState::driverTempChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (std::abs(m_driverTemp - m_state->driverTemp()) < 0.1) return;
        m_syncingFromState = true;
        m_driverTemp = m_state->driverTemp();
        emit driverTempChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::passengerTempChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (std::abs(m_passengerTemp - m_state->passengerTemp()) < 0.1) return;
        m_syncingFromState = true;
        m_passengerTemp = m_state->passengerTemp();
        emit passengerTempChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::fanSpeedChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_fanSpeed == m_state->fanSpeed()) return;
        m_syncingFromState = true;
        m_fanSpeed = m_state->fanSpeed();
        emit fanSpeedChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::airflowModeChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_airflowMode == m_state->airflowMode()) return;
        m_syncingFromState = true;
        m_airflowMode = m_state->airflowMode();
        emit airflowModeChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::acActiveChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_acActive == m_state->acActive()) return;
        m_syncingFromState = true;
        m_acActive = m_state->acActive();
        emit acActiveChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::autoActiveChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_autoActive == m_state->autoActive()) return;
        m_syncingFromState = true;
        m_autoActive = m_state->autoActive();
        emit autoActiveChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::dualModeChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_dualMode == m_state->dualMode()) return;
        m_syncingFromState = true;
        m_dualMode = m_state->dualMode();
        emit dualModeChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::recircActiveChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_recircActive == m_state->recircActive()) return;
        m_syncingFromState = true;
        m_recircActive = m_state->recircActive();
        emit recircActiveChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::frontDefrostActiveChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_frontDefrostActive == m_state->frontDefrostActive()) return;
        m_syncingFromState = true;
        m_frontDefrostActive = m_state->frontDefrostActive();
        emit frontDefrostActiveChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::rearDefrostActiveChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_rearDefrostActive == m_state->rearDefrostActive()) return;
        m_syncingFromState = true;
        m_rearDefrostActive = m_state->rearDefrostActive();
        emit rearDefrostActiveChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::driverSeatHeatChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_driverSeatHeat == m_state->driverSeatHeat()) return;
        m_syncingFromState = true;
        m_driverSeatHeat = m_state->driverSeatHeat();
        emit driverSeatHeatChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::passengerSeatHeatChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_passengerSeatHeat == m_state->passengerSeatHeat()) return;
        m_syncingFromState = true;
        m_passengerSeatHeat = m_state->passengerSeatHeat();
        emit passengerSeatHeatChanged();
        m_syncingFromState = false;
    });

    connect(m_state, &VehicleState::ambientTempChanged, this, [this]() {
        if (m_syncingFromState) return;
        if (m_outsideTemp == m_state->ambientTemp()) return;
        m_syncingFromState = true;
        m_outsideTemp = m_state->ambientTemp();
        emit outsideTempChanged();
        m_syncingFromState = false;
    });
}

void ClimateRibbonController::syncToVehicleState()
{
    if (!m_state || m_syncingFromState)
        return;

    m_syncingFromState = true;
    m_state->setDriverTemp(m_driverTemp);
    m_state->setPassengerTemp(m_passengerTemp);
    m_state->setFanSpeed(m_fanSpeed);
    m_state->setAirflowMode(m_airflowMode);
    m_state->setAcActive(m_acActive);
    m_state->setAutoActive(m_autoActive);
    m_state->setDualMode(m_dualMode);
    m_state->setRecircActive(m_recircActive);
    m_state->setFrontDefrostActive(m_frontDefrostActive);
    m_state->setRearDefrostActive(m_rearDefrostActive);
    m_state->setDriverSeatHeat(m_driverSeatHeat);
    m_state->setPassengerSeatHeat(m_passengerSeatHeat);
    m_syncingFromState = false;

    m_state->broadcastClimate();
}
