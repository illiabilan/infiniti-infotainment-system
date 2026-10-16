#include "EmulatorController.h"
#include <QDebug>

EmulatorController::EmulatorController(VehicleState *state, QObject *parent)
    : QObject(parent)
    , m_state(state)
{
    // Connect state changes to controller signals
    connect(m_state, &VehicleState::speedChanged, this, &EmulatorController::speedChanged);
    connect(m_state, &VehicleState::rpmChanged, this, &EmulatorController::rpmChanged);
    connect(m_state, &VehicleState::gearChanged, this, &EmulatorController::gearChanged);
    connect(m_state, &VehicleState::fuelLevelChanged, this, &EmulatorController::fuelLevelChanged);
    connect(m_state, &VehicleState::coolantTempChanged, this, &EmulatorController::tempLevelChanged);
    connect(m_state, &VehicleState::ambientTempChanged, this, &EmulatorController::ambientTempChanged);
    connect(m_state, &VehicleState::odoKmChanged, this, &EmulatorController::odoKmChanged);
    connect(m_state, &VehicleState::dteKmChanged, this, &EmulatorController::dteKmChanged);

    connect(m_state, &VehicleState::flPsiChanged, this, &EmulatorController::flPsiChanged);
    connect(m_state, &VehicleState::frPsiChanged, this, &EmulatorController::frPsiChanged);
    connect(m_state, &VehicleState::rlPsiChanged, this, &EmulatorController::rlPsiChanged);
    connect(m_state, &VehicleState::rrPsiChanged, this, &EmulatorController::rrPsiChanged);

    connect(m_state, &VehicleState::parkBrakeActiveChanged, this, &EmulatorController::parkBrakeActiveChanged);
    connect(m_state, &VehicleState::seatbeltActiveChanged, this, &EmulatorController::seatbeltActiveChanged);
    connect(m_state, &VehicleState::absActiveChanged, this, &EmulatorController::absActiveChanged);
    connect(m_state, &VehicleState::tpmsActiveChanged, this, &EmulatorController::tpmsActiveChanged);
    connect(m_state, &VehicleState::batteryActiveChanged, this, &EmulatorController::batteryActiveChanged);
    connect(m_state, &VehicleState::oilActiveChanged, this, &EmulatorController::oilActiveChanged);
    connect(m_state, &VehicleState::checkEngineActiveChanged, this, &EmulatorController::checkEngineActiveChanged);
    connect(m_state, &VehicleState::escActiveChanged, this, &EmulatorController::escActiveChanged);
    connect(m_state, &VehicleState::escOffActiveChanged, this, &EmulatorController::escOffActiveChanged);
    connect(m_state, &VehicleState::steeringActiveChanged, this, &EmulatorController::steeringActiveChanged);
    connect(m_state, &VehicleState::airbagActiveChanged, this, &EmulatorController::airbagActiveChanged);
    connect(m_state, &VehicleState::masterWarningChanged, this, &EmulatorController::masterWarningChanged);
    connect(m_state, &VehicleState::lightWarningChanged, this, &EmulatorController::lightWarningChanged);

    connect(m_state, &VehicleState::leftIndicatorChanged, this, &EmulatorController::leftIndicatorChanged);
    connect(m_state, &VehicleState::rightIndicatorChanged, this, &EmulatorController::rightIndicatorChanged);
    connect(m_state, &VehicleState::highBeamChanged, this, &EmulatorController::highBeamChanged);
    connect(m_state, &VehicleState::lowBeamChanged, this, &EmulatorController::lowBeamChanged);
    connect(m_state, &VehicleState::positionLampChanged, this, &EmulatorController::positionLampChanged);
    connect(m_state, &VehicleState::lightModeChanged, this, &EmulatorController::lightModeChanged);

    connect(m_state, &VehicleState::doorFrontLeftChanged, this, &EmulatorController::doorFrontLeftChanged);
    connect(m_state, &VehicleState::doorFrontRightChanged, this, &EmulatorController::doorFrontRightChanged);
    connect(m_state, &VehicleState::doorRearLeftChanged, this, &EmulatorController::doorRearLeftChanged);
    connect(m_state, &VehicleState::doorRearRightChanged, this, &EmulatorController::doorRearRightChanged);
    connect(m_state, &VehicleState::hoodOpenChanged, this, &EmulatorController::bonnetOpenChanged);
    connect(m_state, &VehicleState::trunkOpenChanged, this, &EmulatorController::trunkOpenChanged);
    connect(m_state, &VehicleState::centralLockChanged, this, &EmulatorController::centralLockChanged);

    connect(m_state, &VehicleState::driverTempChanged, this, &EmulatorController::driverTempChanged);
    connect(m_state, &VehicleState::passengerTempChanged, this, &EmulatorController::passengerTempChanged);
    connect(m_state, &VehicleState::fanSpeedChanged, this, &EmulatorController::fanSpeedChanged);
    connect(m_state, &VehicleState::airflowModeChanged, this, &EmulatorController::airflowModeChanged);
    connect(m_state, &VehicleState::acActiveChanged, this, &EmulatorController::acActiveChanged);
    connect(m_state, &VehicleState::autoActiveChanged, this, &EmulatorController::autoActiveChanged);
    connect(m_state, &VehicleState::dualModeChanged, this, &EmulatorController::dualModeChanged);
    connect(m_state, &VehicleState::recircActiveChanged, this, &EmulatorController::recircActiveChanged);
    connect(m_state, &VehicleState::ecoModeChanged, this, &EmulatorController::ecoActiveChanged);
    connect(m_state, &VehicleState::frontDefrostActiveChanged, this, &EmulatorController::frontDefrostActiveChanged);
    connect(m_state, &VehicleState::rearDefrostActiveChanged, this, &EmulatorController::rearDefrostActiveChanged);
    connect(m_state, &VehicleState::driverSeatHeatChanged, this, &EmulatorController::driverSeatHeatChanged);
    connect(m_state, &VehicleState::passengerSeatHeatChanged, this, &EmulatorController::passengerSeatHeatChanged);

    connect(m_state, &VehicleState::cruiseEnabledChanged, this, &EmulatorController::cruiseEnabledChanged);
    connect(m_state, &VehicleState::rearLeftBuckledChanged, this, &EmulatorController::rearLeftBuckledChanged);
    connect(m_state, &VehicleState::rearRightBuckledChanged, this, &EmulatorController::rearRightBuckledChanged);
    connect(m_state, &VehicleState::rearCenterBuckledChanged, this, &EmulatorController::rearCenterBuckledChanged);
    connect(m_state, &VehicleState::rearAlarmActiveChanged, this, &EmulatorController::rearAlarmActiveChanged);

    connect(m_state, &VehicleState::smartKeyPromptChanged, this, &EmulatorController::smartKeyPromptChanged);
    connect(m_state, &VehicleState::startPedalPromptChanged, this, &EmulatorController::startPedalPromptChanged);
    connect(m_state, &VehicleState::pressStartAgainAlertChanged, this, &EmulatorController::pressStartAgainAlertChanged);
    connect(m_state, &VehicleState::driverAttentionActiveChanged, this, &EmulatorController::driverAttentionActiveChanged);
    connect(m_state, &VehicleState::reduceSpeedAlertChanged, this, &EmulatorController::reduceSpeedAlertChanged);
    connect(m_state, &VehicleState::sunroofAlertActiveChanged, this, &EmulatorController::sunroofAlertActiveChanged);

    connect(m_state, &VehicleState::isMediaPlayingChanged, this, &EmulatorController::isMediaPlayingChanged);
    connect(m_state, &VehicleState::mediaSourceChanged, this, &EmulatorController::mediaSourceChanged);
    connect(m_state, &VehicleState::showMediaPopupChanged, this, &EmulatorController::showMediaPopupChanged);
    connect(m_state, &VehicleState::menuTabChanged, this, &EmulatorController::menuTabChanged);
    connect(m_state, &VehicleState::tripPageChanged, this, &EmulatorController::tripPageChanged);

    connect(m_state, &VehicleState::clusterStateChanged, this, &EmulatorController::clusterStateChanged);
    connect(m_state, &VehicleState::themeColorChanged, this, &EmulatorController::themeColorChanged);

    connect(&m_driveSimTimer, &QTimer::timeout, this, &EmulatorController::onDriveSimulationTick);
}

void EmulatorController::setThemeColor(const QString &c)
{
    m_state->setThemeColor(c);
    m_state->broadcastModeTheme();
}

void EmulatorController::setClusterState(int s)
{
    m_state->setClusterState(s);
    m_state->broadcastModeTheme();
}

void EmulatorController::setSpeed(int s)
{
    m_state->setSpeed(s);
    m_state->broadcastSpeedBrake();
}

void EmulatorController::setRpm(double r)
{
    m_state->setRpm(r);
    m_state->broadcastEngine();
}

void EmulatorController::setGear(const QString &g)
{
    m_state->setGear(g);
    m_state->broadcastGear();
}

void EmulatorController::setFuelLevel(int l)
{
    m_state->setFuelLevel(l);
    m_state->broadcastCoolantFuel();
}

void EmulatorController::setTempLevel(int t)
{
    m_state->setCoolantTemp(t);
    m_state->broadcastCoolantFuel();
}

void EmulatorController::setAmbientTemp(int t)
{
    m_state->setAmbientTemp(t);
    m_state->broadcastCoolantFuel();
}

void EmulatorController::setOdoKm(int o)
{
    m_state->setOdoKm(o);
    m_state->broadcastCoolantFuel();
}

void EmulatorController::setDteKm(int d)
{
    m_state->setDteKm(d);
    m_state->broadcastCoolantFuel();
}

void EmulatorController::setFlPsi(double v)
{
    m_state->setFlPsi(v);
    m_state->broadcastTpms();
}

void EmulatorController::setFrPsi(double v)
{
    m_state->setFrPsi(v);
    m_state->broadcastTpms();
}

void EmulatorController::setRlPsi(double v)
{
    m_state->setRlPsi(v);
    m_state->broadcastTpms();
}

void EmulatorController::setRrPsi(double v)
{
    m_state->setRrPsi(v);
    m_state->broadcastTpms();
}

void EmulatorController::setAllPsi(double psi)
{
    double clamped = std::clamp(psi, 15.0, 50.0);
    m_state->setFlPsi(clamped);
    m_state->setFrPsi(clamped);
    m_state->setRlPsi(clamped);
    m_state->setRrPsi(clamped);
    m_state->broadcastTpms();
}

void EmulatorController::setParkBrakeActive(bool b)
{
    m_state->setParkBrakeActive(b);
    m_state->broadcastSpeedBrake();
}

void EmulatorController::setSeatbeltActive(bool b)
{
    m_state->setSeatbeltActive(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setAbsActive(bool b)
{
    m_state->setAbsActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setTpmsActive(bool b)
{
    m_state->setTpmsActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setBatteryActive(bool b)
{
    m_state->setBatteryActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setOilActive(bool b)
{
    m_state->setOilActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setCheckEngineActive(bool b)
{
    m_state->setCheckEngineActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setEscActive(bool b)
{
    m_state->setEscActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setEscOffActive(bool b)
{
    m_state->setEscOffActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setSteeringActive(bool b)
{
    m_state->setSteeringActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setAirbagActive(bool b)
{
    m_state->setAirbagActive(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setMasterWarning(bool b)
{
    m_state->setMasterWarning(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setLightWarning(bool b)
{
    m_state->setLightWarning(b);
    m_state->broadcastTelltales();
}

void EmulatorController::setAllTelltales(bool active)
{
    setParkBrakeActive(active);
    setSeatbeltActive(active);
    setAbsActive(active);
    setTpmsActive(active);
    setBatteryActive(active);
    setOilActive(active);
    setCheckEngineActive(active);
    setEscActive(active);
    setEscOffActive(active);
    setSteeringActive(active);
    setAirbagActive(active);
    setMasterWarning(active);
    setLightWarning(active);
}

void EmulatorController::setLeftIndicator(bool b)
{
    m_state->setLeftIndicator(b);
    m_state->broadcastLighting();
}

void EmulatorController::setRightIndicator(bool b)
{
    m_state->setRightIndicator(b);
    m_state->broadcastLighting();
}

void EmulatorController::setHighBeam(bool b)
{
    m_state->setHighBeam(b);
    m_state->broadcastLighting();
}

void EmulatorController::setLowBeam(bool b)
{
    m_state->setLowBeam(b);
    m_state->broadcastLighting();
}

void EmulatorController::setPositionLamp(bool b)
{
    m_state->setPositionLamp(b);
    m_state->broadcastLighting();
}

void EmulatorController::setLightMode(int m)
{
    m_state->setLightMode(m);
    m_state->broadcastLighting();
}

void EmulatorController::setDoorFrontLeft(bool b)
{
    m_state->setDoorFrontLeft(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setDoorFrontRight(bool b)
{
    m_state->setDoorFrontRight(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setDoorRearLeft(bool b)
{
    m_state->setDoorRearLeft(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setDoorRearRight(bool b)
{
    m_state->setDoorRearRight(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setBonnetOpen(bool b)
{
    m_state->setHoodOpen(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setTrunkOpen(bool b)
{
    m_state->setTrunkOpen(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setCentralLock(bool b)
{
    m_state->setCentralLock(b);
    m_state->broadcastBodyClosures();
}

void EmulatorController::setAllDoors(bool open)
{
    m_state->setDoorFrontLeft(open);
    m_state->setDoorFrontRight(open);
    m_state->setDoorRearLeft(open);
    m_state->setDoorRearRight(open);
    m_state->setHoodOpen(open);
    m_state->setTrunkOpen(open);
    m_state->broadcastBodyClosures();
}

void EmulatorController::toggleDoorFL()
{
    setDoorFrontLeft(!m_state->doorFrontLeft());
}

void EmulatorController::toggleDoorFR()
{
    setDoorFrontRight(!m_state->doorFrontRight());
}

void EmulatorController::toggleDoorRL()
{
    setDoorRearLeft(!m_state->doorRearLeft());
}

void EmulatorController::toggleDoorRR()
{
    setDoorRearRight(!m_state->doorRearRight());
}

void EmulatorController::setDriverTemp(double t)
{
    m_state->setDriverTemp(t);
    m_state->broadcastClimate();
}

void EmulatorController::setPassengerTemp(double t)
{
    m_state->setPassengerTemp(t);
    m_state->broadcastClimate();
}

void EmulatorController::setFanSpeed(int s)
{
    m_state->setFanSpeed(s);
    m_state->broadcastClimate();
}

void EmulatorController::setAirflowMode(int m)
{
    m_state->setAirflowMode(m);
    m_state->broadcastClimate();
}

void EmulatorController::setAcActive(bool b)
{
    m_state->setAcActive(b);
    m_state->broadcastClimate();
}

void EmulatorController::setAutoActive(bool b)
{
    m_state->setAutoActive(b);
    m_state->broadcastClimate();
}

void EmulatorController::setDualMode(bool b)
{
    m_state->setDualMode(b);
    m_state->broadcastClimate();
}

void EmulatorController::setRecircActive(bool b)
{
    m_state->setRecircActive(b);
    m_state->broadcastClimate();
}

void EmulatorController::setEcoActive(bool b)
{
    m_state->setEcoMode(b);
    m_state->broadcastClimate();
}

void EmulatorController::setFrontDefrostActive(bool b)
{
    m_state->setFrontDefrostActive(b);
    m_state->broadcastClimate();
}

void EmulatorController::setRearDefrostActive(bool b)
{
    m_state->setRearDefrostActive(b);
    m_state->broadcastClimate();
}

void EmulatorController::setDriverSeatHeat(int h)
{
    m_state->setDriverSeatHeat(h);
    m_state->broadcastClimate();
}

void EmulatorController::setPassengerSeatHeat(int h)
{
    m_state->setPassengerSeatHeat(h);
    m_state->broadcastClimate();
}

void EmulatorController::setCruiseEnabled(bool b)
{
    m_state->setCruiseEnabled(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setRearLeftBuckled(bool b)
{
    m_state->setRearLeftBuckled(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setRearRightBuckled(bool b)
{
    m_state->setRearRightBuckled(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setRearCenterBuckled(bool b)
{
    m_state->setRearCenterBuckled(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setRearAlarmActive(bool b)
{
    m_state->setRearAlarmActive(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::stopRearAlarm()
{
    setRearAlarmActive(false);
}

void EmulatorController::setSmartKeyPrompt(int p)
{
    m_state->setSmartKeyPrompt(p);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::showSmartKeyAlert(int type)
{
    setSmartKeyPrompt(type);
}

void EmulatorController::setStartPedalPrompt(int p)
{
    m_state->setStartPedalPrompt(p);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::showStartPedalAlert(int prompt)
{
    setStartPedalPrompt(prompt);
}

void EmulatorController::setPressStartAgainAlert(bool b)
{
    m_state->setPressStartAgainAlert(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setDriverAttentionActive(bool b)
{
    m_state->setDriverAttentionActive(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setReduceSpeedAlert(bool b)
{
    m_state->setReduceSpeedAlert(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setSunroofAlertActive(bool b)
{
    m_state->setSunroofOpen(b);
    m_state->setSunroofAlertActive(b);
    m_state->broadcastOccupantsAlerts();
}

void EmulatorController::setIsMediaPlaying(bool b)
{
    m_state->setIsMediaPlaying(b);
    m_state->broadcastInfotainmentMfd(0);
}

void EmulatorController::setMediaSource(const QString &s)
{
    m_state->setMediaSource(s);
    m_state->broadcastInfotainmentMfd(0);
}

void EmulatorController::setShowMediaPopup(bool b)
{
    m_state->setShowMediaPopup(b);
    m_state->broadcastInfotainmentMfd(0);
}

void EmulatorController::toggleMediaPlayback()
{
    setIsMediaPlaying(!isMediaPlaying());
}

void EmulatorController::nextMediaTrack()
{
    m_state->broadcastInfotainmentMfd(1);
}

void EmulatorController::prevMediaTrack()
{
    m_state->broadcastInfotainmentMfd(2);
}

void EmulatorController::triggerMediaPopup()
{
    m_state->setShowMediaPopup(true);
    m_state->broadcastInfotainmentMfd(4);
}

void EmulatorController::setMenuTab(int t)
{
    m_state->setMenuTab(t);
    m_state->broadcastInfotainmentMfd(0);
}

void EmulatorController::setTripPage(int p)
{
    m_state->setTripPage(p);
    m_state->broadcastInfotainmentMfd(0);
}

void EmulatorController::resetActiveTripPage()
{
    resetTrip();
    m_state->broadcastInfotainmentMfd(3);
}

void EmulatorController::deflateFL() { setFlPsi(24.0); }
void EmulatorController::deflateFR() { setFrPsi(24.0); }
void EmulatorController::deflateRL() { setRlPsi(24.0); }
void EmulatorController::deflateRR() { setRrPsi(24.0); }
void EmulatorController::punctureFL() { setFlPsi(16.0); }
void EmulatorController::punctureFR() { setFrPsi(16.0); }
void EmulatorController::punctureRL() { setRlPsi(16.0); }
void EmulatorController::punctureRR() { setRrPsi(16.0); }
void EmulatorController::setAllTiresOK() { setAllPsi(35.0); }

void EmulatorController::toggleCruise()
{
    setCruiseEnabled(!cruiseEnabled());
}

void EmulatorController::cruiseCancel()
{
    setCruiseEnabled(false);
}

void EmulatorController::cruiseResPlus()
{
    setSpeed(std::min(180, speed() + 2));
}

void EmulatorController::cruiseSetMinus()
{
    setSpeed(std::max(0, speed() - 2));
}

void EmulatorController::setIgnitionOnDirect()
{
    triggerStartupSequence();
}

void EmulatorController::setIgnitionOffDirect()
{
    setClusterState(5); // Off
    setSpeed(0);
    setRpm(0.0);
    setAllTelltales(false);
}

void EmulatorController::driveDemo()
{
    m_isDemoDriving = !m_isDemoDriving;
    emit isDemoDrivingChanged();

    if (m_isDemoDriving) {
        m_demoCycleTime = 0.0;
        m_demoScenario = "Starting Demo Drive...";
        emit demoScenarioChanged();
        m_driveSimTimer.start(33);
    } else {
        m_driveSimTimer.stop();
        m_demoScenario = "Idle / Parked";
        emit demoScenarioChanged();
        setSpeed(0);
        setRpm(0.8);
        setGear("P");
        setLeftIndicator(false);
        setRightIndicator(false);
        setParkBrakeActive(true);
    }
}

void EmulatorController::triggerStartupSequence()
{
    setClusterState(1); // Startup
    setSpeed(0);
    setRpm(0.0);
    setAllTelltales(false);
    setGear("P");

    QTimer::singleShot(2500, this, [this]() {
        setClusterState(2); // Boot check
        setAllTelltales(true);
        QTimer::singleShot(1500, this, [this]() {
            setClusterState(3); // Normal trip
            setAllTelltales(false);
            setRpm(0.8);
            setParkBrakeActive(true);
            setSeatbeltActive(true);
        });
    });
}

void EmulatorController::triggerShutdown()
{
    setClusterState(4); // Goodbye
    setSpeed(0);
    setRpm(0.0);
    setAllTelltales(false);
    QTimer::singleShot(2000, this, [this]() {
        setClusterState(5); // Off
    });
}

void EmulatorController::cycleGear()
{
    const QString gears[] = { "P", "R", "N", "D", "M1", "M2", "M3", "M4", "M5" };
    int count = sizeof(gears) / sizeof(gears[0]);
    int cur = 0;
    for (int i = 0; i < count; ++i) {
        if (gear() == gears[i]) {
            cur = i;
            break;
        }
    }
    setGear(gears[(cur + 1) % count]);
}

void EmulatorController::cycleLightMode()
{
    int nextMode = (lightMode() + 1) % 4;
    setLightMode(nextMode);
    switch (nextMode) {
    case 0: // OFF
        setLowBeam(false);
        setPositionLamp(false);
        break;
    case 1: // AUTO
        setPositionLamp(true);
        setLowBeam(true);
        break;
    case 2: // POSITION
        setPositionLamp(true);
        setLowBeam(false);
        break;
    case 3: // LOW BEAM
        setPositionLamp(true);
        setLowBeam(true);
        break;
    }
}

void EmulatorController::resetTrip()
{
    m_tripKm = 0.0;
    m_tripEconomy = 0.0;
    emit tripKmChanged();
    emit tripEconomyChanged();
}

void EmulatorController::nextTripPage()
{
    setTripPage((tripPage() + 1) % 4);
}

void EmulatorController::prevTripPage()
{
    setTripPage((tripPage() - 1 + 4) % 4);
}

void EmulatorController::triggerInfoMenu()
{
    m_state->setMenuTab((menuTab() + 1) % 4);
    m_state->broadcastInfotainmentMfd(5);
}

void EmulatorController::onDriveSimulationTick()
{
    if (!m_isDemoDriving) return;

    m_demoCycleTime += 0.033;
    double t = std::fmod(m_demoCycleTime, 60.0);

    double targetSpeed = 0.0;
    QString scenario;

    if (t < 5.0) {
        targetSpeed = 0.0;
        scenario = "Disengaging Park Brake & Shifting D";
        setGear("D");
        setParkBrakeActive(false);
        setSeatbeltActive(true);
    } else if (t < 15.0) {
        double p = (t - 5.0) / 10.0;
        targetSpeed = 45.0 * p;
        scenario = QString("City Cruising (%1 km/h)").arg(static_cast<int>(targetSpeed));
    } else if (t < 25.0) {
        double p = (t - 15.0) / 10.0;
        targetSpeed = 45.0 + 40.0 * p;
        scenario = QString("Highway Acceleration (%1 km/h)").arg(static_cast<int>(targetSpeed));
        setLeftIndicator(p < 0.4);
    } else if (t < 40.0) {
        targetSpeed = 85.0 + 5.0 * std::sin(t * 0.5);
        scenario = "Highway Cruise at Speed";
        setLeftIndicator(false);
        setRightIndicator(false);
    } else if (t < 52.0) {
        double p = (t - 40.0) / 12.0;
        targetSpeed = 90.0 * (1.0 - p);
        scenario = QString("Decelerating to Stop (%1 km/h)").arg(static_cast<int>(targetSpeed));
        setRightIndicator(p < 0.5);
    } else {
        targetSpeed = 0.0;
        scenario = "Stopped at Traffic Light (Idle)";
        setRightIndicator(false);
    }

    double curSpeed = speed();
    double nextSpeedD = curSpeed + (targetSpeed - curSpeed) * 0.1;
    int nextSpeed = static_cast<int>(std::round(nextSpeedD));
    if (nextSpeed < 0) nextSpeed = 0;
    setSpeed(nextSpeed);

    // Dynamic RPM calculation
    double calculatedRpm = 0.8;
    if (nextSpeed > 0) {
        int gNum = 1;
        if (nextSpeed < 20) gNum = 1;
        else if (nextSpeed < 40) gNum = 2;
        else if (nextSpeed < 65) gNum = 3;
        else if (nextSpeed < 90) gNum = 4;
        else gNum = 5;

        setGear(QString("D%1").arg(gNum));
        double baseSpd = (gNum == 1) ? 0 : (gNum == 2 ? 20 : (gNum == 3 ? 40 : (gNum == 4 ? 65 : 90)));
        calculatedRpm = 1.2 + ((nextSpeed - baseSpd) / 25.0) * 3.5;
        if (calculatedRpm > 6.8) calculatedRpm = 6.8;
        if (calculatedRpm < 1.0) calculatedRpm = 1.0;
    } else {
        setGear("D");
        calculatedRpm = 0.8;
    }
    setRpm(calculatedRpm);

    m_tripKm += (nextSpeed / 3600.0) * 0.033;
    emit tripKmChanged();

    if (m_demoScenario != scenario) {
        m_demoScenario = scenario;
        emit demoScenarioChanged();
    }
}
