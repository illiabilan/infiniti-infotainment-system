#include "VehicleState.h"
#include "NissanCanDefinitions.h"
#include "CanInterface.h"
#include <cmath>

static VehicleState *s_instance = nullptr;

VehicleState::VehicleState(QObject *parent)
    : QObject(parent)
{
    if (!s_instance) {
        s_instance = this;
    }
}

VehicleState* VehicleState::instance()
{
    if (!s_instance) {
        s_instance = new VehicleState();
    }
    return s_instance;
}

void VehicleState::attachCanInterface(CanInterface *canInterface)
{
    if (m_canInterface == canInterface) return;

    if (m_canInterface) {
        disconnect(m_canInterface, &CanInterface::frameReceived, this, &VehicleState::processFrame);
    }

    m_canInterface = canInterface;

    if (m_canInterface) {
        connect(m_canInterface, &CanInterface::frameReceived, this, &VehicleState::processFrame);
    }
}

bool VehicleState::isAnyDoorOpen() const
{
    return m_doorFrontLeft || m_doorFrontRight || m_doorRearLeft || m_doorRearRight || m_hoodOpen || m_trunkOpen;
}

void VehicleState::processFrame(const CanFrame &frame)
{
    switch (frame.canId) {
    case NissanCan::ID_ENGINE_RPM_THROTTLE: {
        double rpm = 0.0;
        double throttle = 0.0;
        bool running = false;
        NissanCan::decodeEngine(frame, rpm, throttle, running);
        setRpm(rpm > 16.0 ? rpm / 1000.0 : rpm);
        setThrottlePercent(throttle);
        setEngineRunning(running);
        break;
    }

    case NissanCan::ID_VEHICLE_SPEED_BRAKE: {
        int speed = 0;
        bool brake = false;
        bool park = false;
        NissanCan::decodeSpeedBrake(frame, speed, brake, park);
        setSpeed(speed);
        setBrakePressed(brake);
        setParkBrakeActive(park);
        break;
    }

    case NissanCan::ID_TRANSMISSION_GEAR: {
        std::string g = NissanCan::decodeGear(frame);
        setGear(QString::fromStdString(g));
        break;
    }

    case NissanCan::ID_COOLANT_TEMP_FUEL: {
        int coolant = 0;
        int fuel = 0;
        int ambient = 0;
        int odo = 0;
        int dte = 0;
        NissanCan::decodeCoolantFuel(frame, coolant, fuel, ambient, odo, dte);
        setCoolantTemp(coolant);
        setFuelLevel(fuel);
        setAmbientTemp(ambient);
        setOdoKm(odo);
        setDteKm(dte);
        break;
    }

    case NissanCan::ID_TPMS_PRESSURES: {
        double fl = 0.0;
        double fr = 0.0;
        double rl = 0.0;
        double rr = 0.0;
        NissanCan::decodeTpms(frame, fl, fr, rl, rr);
        setFlPsi(fl);
        setFrPsi(fr);
        setRlPsi(rl);
        setRrPsi(rr);
        setTpmsActive(fl < 28.0 || fr < 28.0 || rl < 28.0 || rr < 28.0);
        break;
    }

    case NissanCan::ID_BODY_CLOSURES: {
        bool fl = false, fr = false, rl = false, rr = false;
        bool hood = false, trunk = false, lock = false;
        bool dSeatbelt = false, pSeatbelt = false;
        NissanCan::decodeBodyClosures(frame, fl, fr, rl, rr, hood, trunk, lock, dSeatbelt, pSeatbelt);
        setDoorFrontLeft(fl);
        setDoorFrontRight(fr);
        setDoorRearLeft(rl);
        setDoorRearRight(rr);
        setHoodOpen(hood);
        setTrunkOpen(trunk);
        setCentralLock(lock);
        setSeatbeltActive(dSeatbelt);
        setPassSeatbeltActive(pSeatbelt);
        break;
    }

    case NissanCan::ID_LIGHTING_INDICATORS: {
        bool leftInd = false, rightInd = false;
        bool lowB = false, highB = false, posL = false;
        int mode = 0;
        NissanCan::decodeLighting(frame, leftInd, rightInd, lowB, highB, posL, mode);
        setLeftIndicator(leftInd);
        setRightIndicator(rightInd);
        setLowBeam(lowB);
        setHighBeam(highB);
        setPositionLamp(posL);
        setLightMode(mode);
        break;
    }

    case NissanCan::ID_TELLTALES_WARNINGS: {
        bool abs = false, airbag = false, batt = false, oil = false;
        bool checkEng = false, esc = false, escOff = false;
        bool steering = false, master = false;
        bool lightWarn = false, tpms = false;
        NissanCan::decodeTelltales(frame, abs, airbag, batt, oil, checkEng, esc, escOff, steering, master, lightWarn, tpms);
        setAbsActive(abs);
        setAirbagActive(airbag);
        setBatteryActive(batt);
        setOilActive(oil);
        setCheckEngineActive(checkEng);
        setEscActive(esc);
        setEscOffActive(escOff);
        setSteeringActive(steering);
        setMasterWarning(master);
        setLightWarning(lightWarn);
        setTpmsActive(tpms);
        break;
    }

    case NissanCan::ID_OCCUPANTS_ALERTS: {
        bool cruise = false, rl = false, rc = false, rr = false, rAlarm = false;
        int skPrompt = 0, spPrompt = 0;
        bool pressAgain = false, driverAtt = false, redSpeed = false;
        bool srOpen = false, srAlert = false;
        NissanCan::decodeOccupantsAlerts(frame, cruise, rl, rc, rr, rAlarm, skPrompt, spPrompt,
                                         pressAgain, driverAtt, redSpeed, srOpen, srAlert);
        setCruiseEnabled(cruise);
        setRearLeftBuckled(rl);
        setRearCenterBuckled(rc);
        setRearRightBuckled(rr);
        setRearAlarmActive(rAlarm);
        setSmartKeyPrompt(skPrompt);
        setStartPedalPrompt(spPrompt);
        setPressStartAgainAlert(pressAgain);
        setDriverAttentionActive(driverAtt);
        setReduceSpeedAlert(redSpeed);
        setSunroofOpen(srOpen);
        setSunroofAlertActive(srAlert);
        break;
    }

    case NissanCan::ID_INFOTAINMENT_MFD: {
        bool playing = false, popup = false;
        std::string source;
        int mTab = 0, tPage = 0;
        uint8_t cmd = 0;
        NissanCan::decodeInfotainmentMfd(frame, playing, source, popup, mTab, tPage, cmd);
        setIsMediaPlaying(playing);
        setMediaSource(QString::fromStdString(source));
        setShowMediaPopup(popup);
        setMenuTab(mTab);
        setTripPage(tPage);
        if (cmd == 1) emit mediaNextRequested();
        else if (cmd == 2) emit mediaPrevRequested();
        else if (cmd == 3) emit tripResetRequested();
        else if (cmd == 4) emit mediaPopupRequested();
        else if (cmd == 5) emit infoMenuRequested();
        break;
    }

    case NissanCan::ID_CLIMATE_HVAC: {
        double dTemp = 0.0, pTemp = 0.0;
        int fan = 0, airflow = 0;
        bool ac = false, autoMode = false, dual = false, recirc = false;
        bool fDefrost = false, rDefrost = false;
        int dHeat = 0, pHeat = 0;
        bool eco = false;
        NissanCan::decodeClimate(frame, dTemp, pTemp, fan, airflow, ac, autoMode, dual, recirc,
                                 fDefrost, rDefrost, dHeat, pHeat, eco);
        setDriverTemp(dTemp);
        setPassengerTemp(pTemp);
        setFanSpeed(fan);
        setAirflowMode(airflow);
        setAcActive(ac);
        setAutoActive(autoMode);
        setDualMode(dual);
        setRecircActive(recirc);
        setEcoMode(eco);
        setFrontDefrostActive(fDefrost);
        setRearDefrostActive(rDefrost);
        setDriverSeatHeat(dHeat);
        setPassengerSeatHeat(pHeat);
        break;
    }

    case NissanCan::ID_VEHICLE_MODE_THEME: {
        int state = 0;
        std::string theme;
        NissanCan::decodeModeTheme(frame, state, theme);
        setClusterState(state);
        setThemeColor(QString::fromStdString(theme));
        break;
    }

    default:
        break;
    }
}

// Setters
void VehicleState::setSpeed(int speed)
{
    if (m_speed == speed) return;
    m_speed = speed;
    emit speedChanged();
}

void VehicleState::setRpm(double rpm)
{
    if (std::abs(m_rpm - rpm) < 0.1) return;
    m_rpm = rpm;
    emit rpmChanged();
}

void VehicleState::setGear(const QString &gear)
{
    if (m_gear == gear) return;
    m_gear = gear;
    emit gearChanged();
}

void VehicleState::setFuelLevel(int level)
{
    if (m_fuelLevel == level) return;
    m_fuelLevel = level;
    emit fuelLevelChanged();
}

void VehicleState::setCoolantTemp(int temp)
{
    if (m_coolantTemp == temp) return;
    m_coolantTemp = temp;
    emit coolantTempChanged();
}

void VehicleState::setAmbientTemp(int temp)
{
    if (m_ambientTemp == temp) return;
    m_ambientTemp = temp;
    emit ambientTempChanged();
}

void VehicleState::setOdoKm(int odo)
{
    if (m_odoKm == odo) return;
    m_odoKm = odo;
    emit odoKmChanged();
}

void VehicleState::setDteKm(int dte)
{
    if (m_dteKm == dte) return;
    m_dteKm = dte;
    emit dteKmChanged();
}

void VehicleState::setThrottlePercent(double throttle)
{
    if (std::abs(m_throttlePercent - throttle) < 0.1) return;
    m_throttlePercent = throttle;
    emit throttlePercentChanged();
}

void VehicleState::setEngineRunning(bool running)
{
    if (m_engineRunning == running) return;
    m_engineRunning = running;
    emit engineRunningChanged();
}

void VehicleState::setFlPsi(double val)
{
    if (std::abs(m_flPsi - val) < 0.1) return;
    m_flPsi = val;
    emit flPsiChanged();
}

void VehicleState::setFrPsi(double val)
{
    if (std::abs(m_frPsi - val) < 0.1) return;
    m_frPsi = val;
    emit frPsiChanged();
}

void VehicleState::setRlPsi(double val)
{
    if (std::abs(m_rlPsi - val) < 0.1) return;
    m_rlPsi = val;
    emit rlPsiChanged();
}

void VehicleState::setRrPsi(double val)
{
    if (std::abs(m_rrPsi - val) < 0.1) return;
    m_rrPsi = val;
    emit rrPsiChanged();
}

void VehicleState::setBrakePressed(bool val)
{
    if (m_brakePressed == val) return;
    m_brakePressed = val;
    emit brakePressedChanged();
}

void VehicleState::setParkBrakeActive(bool val)
{
    if (m_parkBrakeActive == val) return;
    m_parkBrakeActive = val;
    emit parkBrakeActiveChanged();
}

void VehicleState::setSeatbeltActive(bool val)
{
    if (m_seatbeltActive == val) return;
    m_seatbeltActive = val;
    emit seatbeltActiveChanged();
}

void VehicleState::setPassSeatbeltActive(bool val)
{
    if (m_passSeatbeltActive == val) return;
    m_passSeatbeltActive = val;
    emit passSeatbeltActiveChanged();
}

void VehicleState::setAbsActive(bool val)
{
    if (m_absActive == val) return;
    m_absActive = val;
    emit absActiveChanged();
}

void VehicleState::setBatteryActive(bool val)
{
    if (m_batteryActive == val) return;
    m_batteryActive = val;
    emit batteryActiveChanged();
}

void VehicleState::setOilActive(bool val)
{
    if (m_oilActive == val) return;
    m_oilActive = val;
    emit oilActiveChanged();
}

void VehicleState::setCheckEngineActive(bool val)
{
    if (m_checkEngineActive == val) return;
    m_checkEngineActive = val;
    emit checkEngineActiveChanged();
}

void VehicleState::setEscActive(bool val)
{
    if (m_escActive == val) return;
    m_escActive = val;
    emit escActiveChanged();
}

void VehicleState::setEscOffActive(bool val)
{
    if (m_escOffActive == val) return;
    m_escOffActive = val;
    emit escOffActiveChanged();
}

void VehicleState::setSteeringActive(bool val)
{
    if (m_steeringActive == val) return;
    m_steeringActive = val;
    emit steeringActiveChanged();
}

void VehicleState::setAirbagActive(bool val)
{
    if (m_airbagActive == val) return;
    m_airbagActive = val;
    emit airbagActiveChanged();
}

void VehicleState::setTpmsActive(bool val)
{
    if (m_tpmsActive == val) return;
    m_tpmsActive = val;
    emit tpmsActiveChanged();
}

void VehicleState::setMasterWarning(bool val)
{
    if (m_masterWarning == val) return;
    m_masterWarning = val;
    emit masterWarningChanged();
}

void VehicleState::setLightWarning(bool val)
{
    if (m_lightWarning == val) return;
    m_lightWarning = val;
    emit lightWarningChanged();
}

void VehicleState::setDoorFrontLeft(bool val)
{
    if (m_doorFrontLeft == val) return;
    m_doorFrontLeft = val;
    emit doorFrontLeftChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleState::setDoorFrontRight(bool val)
{
    if (m_doorFrontRight == val) return;
    m_doorFrontRight = val;
    emit doorFrontRightChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleState::setDoorRearLeft(bool val)
{
    if (m_doorRearLeft == val) return;
    m_doorRearLeft = val;
    emit doorRearLeftChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleState::setDoorRearRight(bool val)
{
    if (m_doorRearRight == val) return;
    m_doorRearRight = val;
    emit doorRearRightChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleState::setHoodOpen(bool val)
{
    if (m_hoodOpen == val) return;
    m_hoodOpen = val;
    emit hoodOpenChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleState::setTrunkOpen(bool val)
{
    if (m_trunkOpen == val) return;
    m_trunkOpen = val;
    emit trunkOpenChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleState::setCentralLock(bool val)
{
    if (m_centralLock == val) return;
    m_centralLock = val;
    emit centralLockChanged();
}

void VehicleState::setSunroofOpen(bool val)
{
    if (m_sunroofOpen == val) return;
    m_sunroofOpen = val;
    emit sunroofOpenChanged();
}

void VehicleState::setCruiseEnabled(bool val)
{
    if (m_cruiseEnabled == val) return;
    m_cruiseEnabled = val;
    emit cruiseEnabledChanged();
}

void VehicleState::setRearLeftBuckled(bool val)
{
    if (m_rearLeftBuckled == val) return;
    m_rearLeftBuckled = val;
    emit rearLeftBuckledChanged();
}

void VehicleState::setRearCenterBuckled(bool val)
{
    if (m_rearCenterBuckled == val) return;
    m_rearCenterBuckled = val;
    emit rearCenterBuckledChanged();
}

void VehicleState::setRearRightBuckled(bool val)
{
    if (m_rearRightBuckled == val) return;
    m_rearRightBuckled = val;
    emit rearRightBuckledChanged();
}

void VehicleState::setRearAlarmActive(bool val)
{
    if (m_rearAlarmActive == val) return;
    m_rearAlarmActive = val;
    emit rearAlarmActiveChanged();
}

void VehicleState::setSmartKeyPrompt(int val)
{
    if (m_smartKeyPrompt == val) return;
    m_smartKeyPrompt = val;
    emit smartKeyPromptChanged();
}

void VehicleState::setStartPedalPrompt(int val)
{
    if (m_startPedalPrompt == val) return;
    m_startPedalPrompt = val;
    emit startPedalPromptChanged();
}

void VehicleState::setPressStartAgainAlert(bool val)
{
    if (m_pressStartAgainAlert == val) return;
    m_pressStartAgainAlert = val;
    emit pressStartAgainAlertChanged();
}

void VehicleState::setDriverAttentionActive(bool val)
{
    if (m_driverAttentionActive == val) return;
    m_driverAttentionActive = val;
    emit driverAttentionActiveChanged();
}

void VehicleState::setReduceSpeedAlert(bool val)
{
    if (m_reduceSpeedAlert == val) return;
    m_reduceSpeedAlert = val;
    emit reduceSpeedAlertChanged();
}

void VehicleState::setSunroofAlertActive(bool val)
{
    if (m_sunroofAlertActive == val) return;
    m_sunroofAlertActive = val;
    emit sunroofAlertActiveChanged();
}

void VehicleState::setIsMediaPlaying(bool val)
{
    if (m_isMediaPlaying == val) return;
    m_isMediaPlaying = val;
    emit isMediaPlayingChanged();
}

void VehicleState::setMediaSource(const QString &val)
{
    if (m_mediaSource == val) return;
    m_mediaSource = val;
    emit mediaSourceChanged();
}

void VehicleState::setShowMediaPopup(bool val)
{
    if (m_showMediaPopup == val) return;
    m_showMediaPopup = val;
    emit showMediaPopupChanged();
}

void VehicleState::setMenuTab(int val)
{
    if (m_menuTab == val) return;
    m_menuTab = val;
    emit menuTabChanged();
}

void VehicleState::setTripPage(int val)
{
    if (m_tripPage == val) return;
    m_tripPage = val;
    emit tripPageChanged();
}

void VehicleState::setLeftIndicator(bool val)
{
    if (m_leftIndicator == val) return;
    m_leftIndicator = val;
    emit leftIndicatorChanged();
}

void VehicleState::setRightIndicator(bool val)
{
    if (m_rightIndicator == val) return;
    m_rightIndicator = val;
    emit rightIndicatorChanged();
}

void VehicleState::setLowBeam(bool val)
{
    if (m_lowBeam == val) return;
    m_lowBeam = val;
    emit lowBeamChanged();
}

void VehicleState::setHighBeam(bool val)
{
    if (m_highBeam == val) return;
    m_highBeam = val;
    emit highBeamChanged();
}

void VehicleState::setPositionLamp(bool val)
{
    if (m_positionLamp == val) return;
    m_positionLamp = val;
    emit positionLampChanged();
}

void VehicleState::setLightMode(int val)
{
    if (m_lightMode == val) return;
    m_lightMode = val;
    emit lightModeChanged();
}

void VehicleState::setDriverTemp(double val)
{
    if (std::abs(m_driverTemp - val) < 0.1) return;
    m_driverTemp = val;
    emit driverTempChanged();
}

void VehicleState::setPassengerTemp(double val)
{
    if (std::abs(m_passengerTemp - val) < 0.1) return;
    m_passengerTemp = val;
    emit passengerTempChanged();
}

void VehicleState::setFanSpeed(int val)
{
    if (m_fanSpeed == val) return;
    m_fanSpeed = val;
    emit fanSpeedChanged();
}

void VehicleState::setAirflowMode(int val)
{
    if (m_airflowMode == val) return;
    m_airflowMode = val;
    emit airflowModeChanged();
}

void VehicleState::setAcActive(bool val)
{
    if (m_acActive == val) return;
    m_acActive = val;
    emit acActiveChanged();
}

void VehicleState::setAutoActive(bool val)
{
    if (m_autoActive == val) return;
    m_autoActive = val;
    emit autoActiveChanged();
}

void VehicleState::setDualMode(bool val)
{
    if (m_dualMode == val) return;
    m_dualMode = val;
    emit dualModeChanged();
}

void VehicleState::setRecircActive(bool val)
{
    if (m_recircActive == val) return;
    m_recircActive = val;
    emit recircActiveChanged();
}

void VehicleState::setEcoMode(bool val)
{
    if (m_ecoMode == val) return;
    m_ecoMode = val;
    emit ecoModeChanged();
}

void VehicleState::setFrontDefrostActive(bool val)
{
    if (m_frontDefrostActive == val) return;
    m_frontDefrostActive = val;
    emit frontDefrostActiveChanged();
}

void VehicleState::setRearDefrostActive(bool val)
{
    if (m_rearDefrostActive == val) return;
    m_rearDefrostActive = val;
    emit rearDefrostActiveChanged();
}

void VehicleState::setDriverSeatHeat(int val)
{
    if (m_driverSeatHeat == val) return;
    m_driverSeatHeat = val;
    emit driverSeatHeatChanged();
}

void VehicleState::setPassengerSeatHeat(int val)
{
    if (m_passengerSeatHeat == val) return;
    m_passengerSeatHeat = val;
    emit passengerSeatHeatChanged();
}

void VehicleState::setClusterState(int state)
{
    if (m_clusterState == state) return;
    m_clusterState = state;
    emit clusterStateChanged();
}

void VehicleState::setThemeColor(const QString &color)
{
    if (m_themeColor == color) return;
    m_themeColor = color;
    emit themeColorChanged();
}

// Outbound broadcast helpers
void VehicleState::broadcastEngine()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeEngine(m_rpm <= 16.0 ? m_rpm * 1000.0 : m_rpm, m_throttlePercent, m_engineRunning);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastSpeedBrake()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeSpeedBrake(m_speed, m_brakePressed, m_parkBrakeActive);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastGear()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeGear(m_gear.toStdString());
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastCoolantFuel()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeCoolantFuel(m_coolantTemp, m_fuelLevel, m_ambientTemp, m_odoKm, m_dteKm);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastTpms()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeTpms(m_flPsi, m_frPsi, m_rlPsi, m_rrPsi);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastBodyClosures()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeBodyClosures(m_doorFrontLeft, m_doorFrontRight, m_doorRearLeft, m_doorRearRight,
                                               m_hoodOpen, m_trunkOpen, m_centralLock,
                                               m_seatbeltActive, m_passSeatbeltActive);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastLighting()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeLighting(m_leftIndicator, m_rightIndicator,
                                          m_lowBeam, m_highBeam, m_positionLamp, m_lightMode);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastTelltales()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeTelltales(m_absActive, m_airbagActive, m_batteryActive, m_oilActive,
                                           m_checkEngineActive, m_escActive, m_escOffActive,
                                           m_steeringActive, m_masterWarning,
                                           m_lightWarning, m_tpmsActive);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastOccupantsAlerts()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeOccupantsAlerts(m_cruiseEnabled, m_rearLeftBuckled, m_rearCenterBuckled, m_rearRightBuckled,
                                                  m_rearAlarmActive, m_smartKeyPrompt, m_startPedalPrompt,
                                                  m_pressStartAgainAlert, m_driverAttentionActive, m_reduceSpeedAlert,
                                                  m_sunroofOpen, m_sunroofAlertActive);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastInfotainmentMfd(uint8_t commandAction)
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeInfotainmentMfd(m_isMediaPlaying, m_mediaSource.toStdString(), m_showMediaPopup,
                                                  m_menuTab, m_tripPage, commandAction);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastClimate()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeClimate(m_driverTemp, m_passengerTemp, m_fanSpeed, m_airflowMode,
                                          m_acActive, m_autoActive, m_dualMode, m_recircActive,
                                          m_frontDefrostActive, m_rearDefrostActive,
                                          m_driverSeatHeat, m_passengerSeatHeat, m_ecoMode);
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastModeTheme()
{
    if (!m_canInterface) return;
    CanFrame f = NissanCan::encodeModeTheme(m_clusterState, m_themeColor.toStdString());
    m_canInterface->sendFrame(f);
}

void VehicleState::broadcastAll()
{
    broadcastEngine();
    broadcastSpeedBrake();
    broadcastGear();
    broadcastCoolantFuel();
    broadcastTpms();
    broadcastBodyClosures();
    broadcastLighting();
    broadcastTelltales();
    broadcastOccupantsAlerts();
    broadcastInfotainmentMfd();
    broadcastClimate();
    broadcastModeTheme();
}
