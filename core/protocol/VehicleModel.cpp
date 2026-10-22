#include "VehicleModel.h"
#include <cmath>

VehicleModel::VehicleModel(QObject *parent)
    : QObject(parent)
{
}

void VehicleModel::publish(VehicleModel::Group group, quint8 commandAction)
{
    emit publishRequested(group, commandAction);
}

bool VehicleModel::isSameValue(double current, double incoming) const
{
    return m_exactWrites ? current == incoming : std::abs(current - incoming) < 0.1;
}

bool VehicleModel::isAnyDoorOpen() const
{
    return m_doorFrontLeft || m_doorFrontRight || m_doorRearLeft || m_doorRearRight || m_hoodOpen || m_trunkOpen;
}

// Setters
void VehicleModel::setSpeed(int speed)
{
    if (m_speed == speed) return;
    m_speed = speed;
    emit speedChanged();
}

void VehicleModel::setRpm(double rpm)
{
    if (isSameValue(m_rpm, rpm)) return;
    m_rpm = rpm;
    emit rpmChanged();
}

void VehicleModel::setGear(const QString &gear)
{
    if (m_gear == gear) return;
    m_gear = gear;
    emit gearChanged();
}

void VehicleModel::setFuelLevel(int level)
{
    if (m_fuelLevel == level) return;
    m_fuelLevel = level;
    emit fuelLevelChanged();
}

void VehicleModel::setCoolantTemp(int temp)
{
    if (m_coolantTemp == temp) return;
    m_coolantTemp = temp;
    emit coolantTempChanged();
}

void VehicleModel::setAmbientTemp(int temp)
{
    if (m_ambientTemp == temp) return;
    m_ambientTemp = temp;
    emit ambientTempChanged();
}

void VehicleModel::setOdoKm(int odo)
{
    if (m_odoKm == odo) return;
    m_odoKm = odo;
    emit odoKmChanged();
}

void VehicleModel::setDteKm(int dte)
{
    if (m_dteKm == dte) return;
    m_dteKm = dte;
    emit dteKmChanged();
}

void VehicleModel::setThrottlePercent(double throttle)
{
    if (isSameValue(m_throttlePercent, throttle)) return;
    m_throttlePercent = throttle;
    emit throttlePercentChanged();
}

void VehicleModel::setEngineRunning(bool running)
{
    if (m_engineRunning == running) return;
    m_engineRunning = running;
    emit engineRunningChanged();
}

void VehicleModel::setFlPsi(double val)
{
    if (isSameValue(m_flPsi, val)) return;
    m_flPsi = val;
    emit flPsiChanged();
}

void VehicleModel::setFrPsi(double val)
{
    if (isSameValue(m_frPsi, val)) return;
    m_frPsi = val;
    emit frPsiChanged();
}

void VehicleModel::setRlPsi(double val)
{
    if (isSameValue(m_rlPsi, val)) return;
    m_rlPsi = val;
    emit rlPsiChanged();
}

void VehicleModel::setRrPsi(double val)
{
    if (isSameValue(m_rrPsi, val)) return;
    m_rrPsi = val;
    emit rrPsiChanged();
}

void VehicleModel::setBrakePressed(bool val)
{
    if (m_brakePressed == val) return;
    m_brakePressed = val;
    emit brakePressedChanged();
}

void VehicleModel::setParkBrakeActive(bool val)
{
    if (m_parkBrakeActive == val) return;
    m_parkBrakeActive = val;
    emit parkBrakeActiveChanged();
}

void VehicleModel::setSeatbeltActive(bool val)
{
    if (m_seatbeltActive == val) return;
    m_seatbeltActive = val;
    emit seatbeltActiveChanged();
}

void VehicleModel::setPassSeatbeltActive(bool val)
{
    if (m_passSeatbeltActive == val) return;
    m_passSeatbeltActive = val;
    emit passSeatbeltActiveChanged();
}

void VehicleModel::setAbsActive(bool val)
{
    if (m_absActive == val) return;
    m_absActive = val;
    emit absActiveChanged();
}

void VehicleModel::setBatteryActive(bool val)
{
    if (m_batteryActive == val) return;
    m_batteryActive = val;
    emit batteryActiveChanged();
}

void VehicleModel::setOilActive(bool val)
{
    if (m_oilActive == val) return;
    m_oilActive = val;
    emit oilActiveChanged();
}

void VehicleModel::setCheckEngineActive(bool val)
{
    if (m_checkEngineActive == val) return;
    m_checkEngineActive = val;
    emit checkEngineActiveChanged();
}

void VehicleModel::setEscActive(bool val)
{
    if (m_escActive == val) return;
    m_escActive = val;
    emit escActiveChanged();
}

void VehicleModel::setEscOffActive(bool val)
{
    if (m_escOffActive == val) return;
    m_escOffActive = val;
    emit escOffActiveChanged();
}

void VehicleModel::setSteeringActive(bool val)
{
    if (m_steeringActive == val) return;
    m_steeringActive = val;
    emit steeringActiveChanged();
}

void VehicleModel::setAirbagActive(bool val)
{
    if (m_airbagActive == val) return;
    m_airbagActive = val;
    emit airbagActiveChanged();
}

void VehicleModel::setTpmsActive(bool val)
{
    if (m_tpmsActive == val) return;
    m_tpmsActive = val;
    emit tpmsActiveChanged();
}

void VehicleModel::setMasterWarning(bool val)
{
    if (m_masterWarning == val) return;
    m_masterWarning = val;
    emit masterWarningChanged();
}

void VehicleModel::setLightWarning(bool val)
{
    if (m_lightWarning == val) return;
    m_lightWarning = val;
    emit lightWarningChanged();
}

void VehicleModel::setDoorFrontLeft(bool val)
{
    if (m_doorFrontLeft == val) return;
    m_doorFrontLeft = val;
    emit doorFrontLeftChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleModel::setDoorFrontRight(bool val)
{
    if (m_doorFrontRight == val) return;
    m_doorFrontRight = val;
    emit doorFrontRightChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleModel::setDoorRearLeft(bool val)
{
    if (m_doorRearLeft == val) return;
    m_doorRearLeft = val;
    emit doorRearLeftChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleModel::setDoorRearRight(bool val)
{
    if (m_doorRearRight == val) return;
    m_doorRearRight = val;
    emit doorRearRightChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleModel::setHoodOpen(bool val)
{
    if (m_hoodOpen == val) return;
    m_hoodOpen = val;
    emit hoodOpenChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleModel::setTrunkOpen(bool val)
{
    if (m_trunkOpen == val) return;
    m_trunkOpen = val;
    emit trunkOpenChanged();
    emit isAnyDoorOpenChanged();
}

void VehicleModel::setCentralLock(bool val)
{
    if (m_centralLock == val) return;
    m_centralLock = val;
    emit centralLockChanged();
}

void VehicleModel::setSunroofOpen(bool val)
{
    if (m_sunroofOpen == val) return;
    m_sunroofOpen = val;
    emit sunroofOpenChanged();
}

void VehicleModel::setCruiseEnabled(bool val)
{
    if (m_cruiseEnabled == val) return;
    m_cruiseEnabled = val;
    emit cruiseEnabledChanged();
}

void VehicleModel::setRearLeftBuckled(bool val)
{
    if (m_rearLeftBuckled == val) return;
    m_rearLeftBuckled = val;
    emit rearLeftBuckledChanged();
}

void VehicleModel::setRearCenterBuckled(bool val)
{
    if (m_rearCenterBuckled == val) return;
    m_rearCenterBuckled = val;
    emit rearCenterBuckledChanged();
}

void VehicleModel::setRearRightBuckled(bool val)
{
    if (m_rearRightBuckled == val) return;
    m_rearRightBuckled = val;
    emit rearRightBuckledChanged();
}

void VehicleModel::setRearAlarmActive(bool val)
{
    if (m_rearAlarmActive == val) return;
    m_rearAlarmActive = val;
    emit rearAlarmActiveChanged();
}

void VehicleModel::setSmartKeyPrompt(int val)
{
    if (m_smartKeyPrompt == val) return;
    m_smartKeyPrompt = val;
    emit smartKeyPromptChanged();
}

void VehicleModel::setStartPedalPrompt(int val)
{
    if (m_startPedalPrompt == val) return;
    m_startPedalPrompt = val;
    emit startPedalPromptChanged();
}

void VehicleModel::setPressStartAgainAlert(bool val)
{
    if (m_pressStartAgainAlert == val) return;
    m_pressStartAgainAlert = val;
    emit pressStartAgainAlertChanged();
}

void VehicleModel::setDriverAttentionActive(bool val)
{
    if (m_driverAttentionActive == val) return;
    m_driverAttentionActive = val;
    emit driverAttentionActiveChanged();
}

void VehicleModel::setReduceSpeedAlert(bool val)
{
    if (m_reduceSpeedAlert == val) return;
    m_reduceSpeedAlert = val;
    emit reduceSpeedAlertChanged();
}

void VehicleModel::setSunroofAlertActive(bool val)
{
    if (m_sunroofAlertActive == val) return;
    m_sunroofAlertActive = val;
    emit sunroofAlertActiveChanged();
}

void VehicleModel::setIsMediaPlaying(bool val)
{
    if (m_isMediaPlaying == val) return;
    m_isMediaPlaying = val;
    emit isMediaPlayingChanged();
}

void VehicleModel::setMediaSource(const QString &val)
{
    if (m_mediaSource == val) return;
    m_mediaSource = val;
    emit mediaSourceChanged();
}

void VehicleModel::setShowMediaPopup(bool val)
{
    if (m_showMediaPopup == val) return;
    m_showMediaPopup = val;
    emit showMediaPopupChanged();
}

void VehicleModel::setMenuTab(int val)
{
    if (m_menuTab == val) return;
    m_menuTab = val;
    emit menuTabChanged();
}

void VehicleModel::setTripPage(int val)
{
    if (m_tripPage == val) return;
    m_tripPage = val;
    emit tripPageChanged();
}

void VehicleModel::setLeftIndicator(bool val)
{
    if (m_leftIndicator == val) return;
    m_leftIndicator = val;
    emit leftIndicatorChanged();
}

void VehicleModel::setRightIndicator(bool val)
{
    if (m_rightIndicator == val) return;
    m_rightIndicator = val;
    emit rightIndicatorChanged();
}

void VehicleModel::setLowBeam(bool val)
{
    if (m_lowBeam == val) return;
    m_lowBeam = val;
    emit lowBeamChanged();
}

void VehicleModel::setHighBeam(bool val)
{
    if (m_highBeam == val) return;
    m_highBeam = val;
    emit highBeamChanged();
}

void VehicleModel::setPositionLamp(bool val)
{
    if (m_positionLamp == val) return;
    m_positionLamp = val;
    emit positionLampChanged();
}

void VehicleModel::setLightMode(int val)
{
    if (m_lightMode == val) return;
    m_lightMode = val;
    emit lightModeChanged();
}

void VehicleModel::setDriverTemp(double val)
{
    if (isSameValue(m_driverTemp, val)) return;
    m_driverTemp = val;
    emit driverTempChanged();
}

void VehicleModel::setPassengerTemp(double val)
{
    if (isSameValue(m_passengerTemp, val)) return;
    m_passengerTemp = val;
    emit passengerTempChanged();
}

void VehicleModel::setFanSpeed(int val)
{
    if (m_fanSpeed == val) return;
    m_fanSpeed = val;
    emit fanSpeedChanged();
}

void VehicleModel::setAirflowMode(int val)
{
    if (m_airflowMode == val) return;
    m_airflowMode = val;
    emit airflowModeChanged();
}

void VehicleModel::setAcActive(bool val)
{
    if (m_acActive == val) return;
    m_acActive = val;
    emit acActiveChanged();
}

void VehicleModel::setAutoActive(bool val)
{
    if (m_autoActive == val) return;
    m_autoActive = val;
    emit autoActiveChanged();
}

void VehicleModel::setDualMode(bool val)
{
    if (m_dualMode == val) return;
    m_dualMode = val;
    emit dualModeChanged();
}

void VehicleModel::setRecircActive(bool val)
{
    if (m_recircActive == val) return;
    m_recircActive = val;
    emit recircActiveChanged();
}

void VehicleModel::setEcoMode(bool val)
{
    if (m_ecoMode == val) return;
    m_ecoMode = val;
    emit ecoModeChanged();
}

void VehicleModel::setFrontDefrostActive(bool val)
{
    if (m_frontDefrostActive == val) return;
    m_frontDefrostActive = val;
    emit frontDefrostActiveChanged();
}

void VehicleModel::setRearDefrostActive(bool val)
{
    if (m_rearDefrostActive == val) return;
    m_rearDefrostActive = val;
    emit rearDefrostActiveChanged();
}

void VehicleModel::setDriverSeatHeat(int val)
{
    if (m_driverSeatHeat == val) return;
    m_driverSeatHeat = val;
    emit driverSeatHeatChanged();
}

void VehicleModel::setPassengerSeatHeat(int val)
{
    if (m_passengerSeatHeat == val) return;
    m_passengerSeatHeat = val;
    emit passengerSeatHeatChanged();
}

void VehicleModel::setClusterState(int state)
{
    if (m_clusterState == state) return;
    m_clusterState = state;
    emit clusterStateChanged();
}

void VehicleModel::setThemeColor(const QString &color)
{
    if (m_themeColor == color) return;
    m_themeColor = color;
    emit themeColorChanged();
}

// Outbound broadcast helpers
void VehicleModel::broadcastEngine()
{
    publish(Group::Engine);
}

void VehicleModel::broadcastSpeedBrake()
{
    publish(Group::SpeedBrake);
}

void VehicleModel::broadcastGear()
{
    publish(Group::Gear);
}

void VehicleModel::broadcastCoolantFuel()
{
    publish(Group::CoolantFuel);
}

void VehicleModel::broadcastTpms()
{
    publish(Group::Tpms);
}

void VehicleModel::broadcastBodyClosures()
{
    publish(Group::BodyClosures);
}

void VehicleModel::broadcastLighting()
{
    publish(Group::Lighting);
}

void VehicleModel::broadcastTelltales()
{
    publish(Group::Telltales);
}

void VehicleModel::broadcastOccupantsAlerts()
{
    publish(Group::OccupantsAlerts);
}

void VehicleModel::broadcastInfotainmentMfd(uint8_t commandAction)
{
    publish(Group::InfotainmentMfd, commandAction);
}

void VehicleModel::broadcastClimate()
{
    publish(Group::Climate);
}

void VehicleModel::broadcastModeTheme()
{
    publish(Group::ModeTheme);
}

void VehicleModel::broadcastAll()
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
