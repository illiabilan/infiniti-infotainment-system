#include "ClimateRibbonController.h"

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
}

void ClimateRibbonController::setFanSpeed(int speed)
{
    const int clamped = std::clamp(speed, 0, 7);
    if (m_fanSpeed == clamped)
        return;

    m_fanSpeed = clamped;
    emit fanSpeedChanged();
}

void ClimateRibbonController::setAirflowMode(int mode)
{
    const int clamped = std::clamp(mode, 0, 3);
    if (m_airflowMode == clamped)
        return;

    m_airflowMode = clamped;
    emit airflowModeChanged();
}

void ClimateRibbonController::setAcActive(bool enabled)
{
    if (m_acActive == enabled)
        return;

    m_acActive = enabled;
    emit acActiveChanged();
}

void ClimateRibbonController::setAutoActive(bool enabled)
{
    if (m_autoActive == enabled)
        return;

    m_autoActive = enabled;
    emit autoActiveChanged();
}

void ClimateRibbonController::setRecircActive(bool enabled)
{
    if (m_recircActive == enabled)
        return;

    m_recircActive = enabled;
    emit recircActiveChanged();
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
}

void ClimateRibbonController::setRearDefrostActive(bool enabled)
{
    if (m_rearDefrostActive == enabled)
        return;

    m_rearDefrostActive = enabled;
    emit rearDefrostActiveChanged();
}

void ClimateRibbonController::setDriverSeatHeat(int level)
{
    const int clamped = std::clamp(level, 0, 3);
    if (m_driverSeatHeat == clamped)
        return;

    m_driverSeatHeat = clamped;
    emit driverSeatHeatChanged();
}

void ClimateRibbonController::setPassengerSeatHeat(int level)
{
    const int clamped = std::clamp(level, 0, 3);
    if (m_passengerSeatHeat == clamped)
        return;

    m_passengerSeatHeat = clamped;
    emit passengerSeatHeatChanged();
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
