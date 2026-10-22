/**
 * ============================================================================
 * Project:        Automotive Digital Instrument Cluster HMI
 * File:           ClusterController.cpp
 * Author:         SK Rehan Ahamed
 * Description:    Automotive CAN / ECU Telemetry Controller Implementation
 * Copyright (c) 2026 SK Rehan Ahamed. All rights reserved.
 * ============================================================================
 */

#include "ClusterController.h"
#include <cmath>

ClusterController::ClusterController(QObject *parent)
    : QObject(parent)
{
    connect(&m_sequenceTimer, &QTimer::timeout, this, &ClusterController::onSequenceStep);

    connect(&m_driveSim, &ClusterDriveSimulator::tickRequested, this, [this]() {
        m_driveSim.onSimulationTick(*this, m_trip, m_vehicleState != nullptr);
    });
    connect(&m_driveSim, &ClusterDriveSimulator::isDemoDrivingChanged, this, &ClusterController::isDemoDrivingChanged);
    connect(&m_driveSim, &ClusterDriveSimulator::demoScenarioChanged, this, &ClusterController::demoScenarioChanged);

    // Trip computer signals
    connect(&m_trip, &ClusterTripComputer::tripKmChanged, this, &ClusterController::tripKmChanged);
    connect(&m_trip, &ClusterTripComputer::tripTimeChanged, this, &ClusterController::tripTimeChanged);
    connect(&m_trip, &ClusterTripComputer::tripEconomyChanged, this, &ClusterController::tripEconomyChanged);
    connect(&m_trip, &ClusterTripComputer::tripPageChanged, this, &ClusterController::tripPageChanged);
    connect(&m_trip, &ClusterTripComputer::refuelKmChanged, this, &ClusterController::refuelKmChanged);
    connect(&m_trip, &ClusterTripComputer::refuelTimeChanged, this, &ClusterController::refuelTimeChanged);
    connect(&m_trip, &ClusterTripComputer::refuelEconomyChanged, this, &ClusterController::refuelEconomyChanged);
    connect(&m_trip, &ClusterTripComputer::accumKmChanged, this, &ClusterController::accumKmChanged);
    connect(&m_trip, &ClusterTripComputer::accumTimeChanged, this, &ClusterController::accumTimeChanged);
    connect(&m_trip, &ClusterTripComputer::accumEconomyChanged, this, &ClusterController::accumEconomyChanged);
    connect(&m_trip, &ClusterTripComputer::instantEconomyChanged, this, &ClusterController::instantEconomyChanged);

    // Telltales, TPMS & Lighting signals
    connect(&m_telltales, &ClusterTelltaleController::parkBrakeActiveChanged, this, &ClusterController::parkBrakeActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::seatbeltActiveChanged, this, &ClusterController::seatbeltActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::absActiveChanged, this, &ClusterController::absActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::batteryActiveChanged, this, &ClusterController::batteryActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::oilActiveChanged, this, &ClusterController::oilActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::leftIndicatorChanged, this, &ClusterController::leftIndicatorChanged);
    connect(&m_telltales, &ClusterTelltaleController::rightIndicatorChanged, this, &ClusterController::rightIndicatorChanged);
    connect(&m_telltales, &ClusterTelltaleController::highBeamChanged, this, &ClusterController::highBeamChanged);
    connect(&m_telltales, &ClusterTelltaleController::lowBeamChanged, this, &ClusterController::lowBeamChanged);
    connect(&m_telltales, &ClusterTelltaleController::positionLampChanged, this, &ClusterController::positionLampChanged);
    connect(&m_telltales, &ClusterTelltaleController::masterWarningChanged, this, &ClusterController::masterWarningChanged);
    connect(&m_telltales, &ClusterTelltaleController::lightWarningChanged, this, &ClusterController::lightWarningChanged);
    connect(&m_telltales, &ClusterTelltaleController::escActiveChanged, this, &ClusterController::escActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::escOffActiveChanged, this, &ClusterController::escOffActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::checkEngineActiveChanged, this, &ClusterController::checkEngineActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::smartKeyActiveChanged, this, &ClusterController::smartKeyActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::steeringActiveChanged, this, &ClusterController::steeringActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::airbagActiveChanged, this, &ClusterController::airbagActiveChanged);
    connect(&m_telltales, &ClusterTelltaleController::lightModeChanged, this, &ClusterController::lightModeChanged);
    connect(&m_telltales, &ClusterTelltaleController::showLightPopupChanged, this, &ClusterController::showLightPopupChanged);
    connect(&m_telltales, &ClusterTelltaleController::flPsiChanged, this, &ClusterController::flPsiChanged);
    connect(&m_telltales, &ClusterTelltaleController::frPsiChanged, this, &ClusterController::frPsiChanged);
    connect(&m_telltales, &ClusterTelltaleController::rlPsiChanged, this, &ClusterController::rlPsiChanged);
    connect(&m_telltales, &ClusterTelltaleController::rrPsiChanged, this, &ClusterController::rrPsiChanged);
    connect(&m_telltales, &ClusterTelltaleController::tpmsCalibratedChanged, this, &ClusterController::tpmsCalibratedChanged);
    connect(&m_telltales, &ClusterTelltaleController::tpmsActiveChanged, this, [this]() {
        emit tpmsActiveChanged();
        if (m_telltales.tpmsActive() && m_clusterState == StateNormalTrip) {
            setMenuTab(2);
            setShowMenuTabs(true);
            m_telltales.startTpmsDismissTimer();
        }
    });
    connect(&m_telltales, &ClusterTelltaleController::tpmsDismissTimeout, this, [this]() {
        if (m_clusterState == StateNormalTrip && m_menuTab == 2) {
            setMenuTab(0);
            setShowMenuTabs(false);
        }
    });

    // Alerts, Closures & Cruise signals
    connect(&m_alerts, &ClusterAlertsController::doorFrontLeftChanged, this, &ClusterController::doorFrontLeftChanged);
    connect(&m_alerts, &ClusterAlertsController::doorFrontRightChanged, this, &ClusterController::doorFrontRightChanged);
    connect(&m_alerts, &ClusterAlertsController::doorRearLeftChanged, this, &ClusterController::doorRearLeftChanged);
    connect(&m_alerts, &ClusterAlertsController::doorRearRightChanged, this, &ClusterController::doorRearRightChanged);
    connect(&m_alerts, &ClusterAlertsController::bonnetOpenChanged, this, &ClusterController::bonnetOpenChanged);
    connect(&m_alerts, &ClusterAlertsController::trunkOpenChanged, this, &ClusterController::trunkOpenChanged);
    connect(&m_alerts, &ClusterAlertsController::doorOpenAlertChanged, this, &ClusterController::doorOpenAlertChanged);
    connect(&m_alerts, &ClusterAlertsController::doorOpenSideChanged, this, &ClusterController::doorOpenSideChanged);

    connect(&m_alerts, &ClusterAlertsController::rearLeftBuckledChanged, this, &ClusterController::rearLeftBuckledChanged);
    connect(&m_alerts, &ClusterAlertsController::rearCenterBuckledChanged, this, &ClusterController::rearCenterBuckledChanged);
    connect(&m_alerts, &ClusterAlertsController::rearRightBuckledChanged, this, &ClusterController::rearRightBuckledChanged);
    connect(&m_alerts, &ClusterAlertsController::rearAlarmActiveChanged, this, &ClusterController::rearAlarmActiveChanged);
    connect(&m_alerts, &ClusterAlertsController::rearAlarmSeatChanged, this, &ClusterController::rearAlarmSeatChanged);
    connect(&m_alerts, &ClusterAlertsController::rearSeatBlinkStateChanged, this, &ClusterController::rearSeatBlinkStateChanged);

    connect(&m_alerts, &ClusterAlertsController::reduceSpeedAlertChanged, this, &ClusterController::reduceSpeedAlertChanged);
    connect(&m_alerts, &ClusterAlertsController::driverAttentionActiveChanged, this, &ClusterController::driverAttentionActiveChanged);
    connect(&m_alerts, &ClusterAlertsController::attentionLevelChanged, this, &ClusterController::attentionLevelChanged);
    connect(&m_alerts, &ClusterAlertsController::serviceDueKmChanged, this, &ClusterController::serviceDueKmChanged);
    connect(&m_alerts, &ClusterAlertsController::serviceDueDaysChanged, this, &ClusterController::serviceDueDaysChanged);
    connect(&m_alerts, &ClusterAlertsController::servicePopupActiveChanged, this, &ClusterController::servicePopupActiveChanged);
    connect(&m_alerts, &ClusterAlertsController::sunroofOpenChanged, this, &ClusterController::sunroofOpenChanged);
    connect(&m_alerts, &ClusterAlertsController::sunroofAlertActiveChanged, this, &ClusterController::sunroofAlertActiveChanged);
    connect(&m_alerts, &ClusterAlertsController::smartKeyPromptChanged, this, &ClusterController::smartKeyPromptChanged);
    connect(&m_alerts, &ClusterAlertsController::isgActiveChanged, this, &ClusterController::isgActiveChanged);
    connect(&m_alerts, &ClusterAlertsController::pressStartAgainAlertChanged, this, &ClusterController::pressStartAgainAlertChanged);
    connect(&m_alerts, &ClusterAlertsController::startPedalPromptChanged, this, &ClusterController::startPedalPromptChanged);

    connect(&m_alerts, &ClusterAlertsController::cruiseEnabledChanged, this, &ClusterController::cruiseEnabledChanged);
    connect(&m_alerts, &ClusterAlertsController::cruiseActiveChanged, this, &ClusterController::cruiseActiveChanged);
    connect(&m_alerts, &ClusterAlertsController::cruiseSetSpeedChanged, this, &ClusterController::cruiseSetSpeedChanged);

    connect(&m_alerts, &ClusterAlertsController::playChimeRequested, this, &ClusterController::signalPlayChime);
    connect(&m_alerts, &ClusterAlertsController::warningChimeRequested, this, &ClusterController::signalWarningChime);
    connect(&m_alerts, &ClusterAlertsController::seatbeltChimeRequested, this, &ClusterController::signalSeatbeltChime);
    connect(&m_alerts, &ClusterAlertsController::keyAlertChimeRequested, this, &ClusterController::signalKeyAlertChime);
    connect(&m_alerts, &ClusterAlertsController::speedAlertChimeRequested, this, &ClusterController::signalSpeedAlertChime);

    // Media signals
    connect(&m_media, &ClusterMediaController::showMediaPopupChanged, this, &ClusterController::showMediaPopupChanged);
    connect(&m_media, &ClusterMediaController::mediaSourceChanged, this, &ClusterController::mediaSourceChanged);
    connect(&m_media, &ClusterMediaController::mediaTitleChanged, this, &ClusterController::mediaTitleChanged);
    connect(&m_media, &ClusterMediaController::mediaArtistChanged, this, &ClusterController::mediaArtistChanged);
    connect(&m_media, &ClusterMediaController::isMediaPlayingChanged, this, &ClusterController::isMediaPlayingChanged);
    connect(&m_media, &ClusterMediaController::mediaTrackIndexChanged, this, &ClusterController::mediaTrackIndexChanged);
}

void ClusterController::attachVehicleState(VehicleModel *state)
{
    m_vehicleState = state;
    if (!m_vehicleState) return;

    connect(m_vehicleState, &VehicleModel::speedChanged, this, [this]() { setSpeed(m_vehicleState->speed()); });
    connect(m_vehicleState, &VehicleModel::rpmChanged, this, [this]() { setRpm(m_vehicleState->rpm()); });
    connect(m_vehicleState, &VehicleModel::gearChanged, this, [this]() { setGear(m_vehicleState->gear()); });
    connect(m_vehicleState, &VehicleModel::coolantTempChanged, this, [this]() { setTempLevel(m_vehicleState->coolantTemp()); });
    connect(m_vehicleState, &VehicleModel::fuelLevelChanged, this, [this]() { setFuelLevel(m_vehicleState->fuelLevel()); });
    connect(m_vehicleState, &VehicleModel::ambientTempChanged, this, [this]() { setAmbientTemp(m_vehicleState->ambientTemp()); });
    connect(m_vehicleState, &VehicleModel::odoKmChanged, this, [this]() { setOdoKm(m_vehicleState->odoKm()); });
    connect(m_vehicleState, &VehicleModel::dteKmChanged, this, [this]() { setDteKm(m_vehicleState->dteKm()); });

    connect(m_vehicleState, &VehicleModel::flPsiChanged, this, [this]() { setFlPsi(m_vehicleState->flPsi()); });
    connect(m_vehicleState, &VehicleModel::frPsiChanged, this, [this]() { setFrPsi(m_vehicleState->frPsi()); });
    connect(m_vehicleState, &VehicleModel::rlPsiChanged, this, [this]() { setRlPsi(m_vehicleState->rlPsi()); });
    connect(m_vehicleState, &VehicleModel::rrPsiChanged, this, [this]() { setRrPsi(m_vehicleState->rrPsi()); });

    connect(m_vehicleState, &VehicleModel::parkBrakeActiveChanged, this, [this]() { setParkBrakeActive(m_vehicleState->parkBrakeActive()); });
    connect(m_vehicleState, &VehicleModel::seatbeltActiveChanged, this, [this]() { setSeatbeltActive(m_vehicleState->seatbeltActive()); });
    connect(m_vehicleState, &VehicleModel::absActiveChanged, this, [this]() { setAbsActive(m_vehicleState->absActive()); });
    connect(m_vehicleState, &VehicleModel::tpmsActiveChanged, this, [this]() { setTpmsActive(m_vehicleState->tpmsActive()); });
    connect(m_vehicleState, &VehicleModel::batteryActiveChanged, this, [this]() { setBatteryActive(m_vehicleState->batteryActive()); });
    connect(m_vehicleState, &VehicleModel::oilActiveChanged, this, [this]() { setOilActive(m_vehicleState->oilActive()); });
    connect(m_vehicleState, &VehicleModel::checkEngineActiveChanged, this, [this]() { setCheckEngineActive(m_vehicleState->checkEngineActive()); });
    connect(m_vehicleState, &VehicleModel::escActiveChanged, this, [this]() { setEscActive(m_vehicleState->escActive()); });
    connect(m_vehicleState, &VehicleModel::escOffActiveChanged, this, [this]() { setEscOffActive(m_vehicleState->escOffActive()); });
    connect(m_vehicleState, &VehicleModel::steeringActiveChanged, this, [this]() { setSteeringActive(m_vehicleState->steeringActive()); });
    connect(m_vehicleState, &VehicleModel::airbagActiveChanged, this, [this]() { setAirbagActive(m_vehicleState->airbagActive()); });
    connect(m_vehicleState, &VehicleModel::masterWarningChanged, this, [this]() { setMasterWarning(m_vehicleState->masterWarning()); });
    connect(m_vehicleState, &VehicleModel::lightWarningChanged, this, [this]() { setLightWarning(m_vehicleState->lightWarning()); });

    connect(m_vehicleState, &VehicleModel::leftIndicatorChanged, this, [this]() { setLeftIndicator(m_vehicleState->leftIndicator()); });
    connect(m_vehicleState, &VehicleModel::rightIndicatorChanged, this, [this]() { setRightIndicator(m_vehicleState->rightIndicator()); });
    connect(m_vehicleState, &VehicleModel::highBeamChanged, this, [this]() { setHighBeam(m_vehicleState->highBeam()); });
    connect(m_vehicleState, &VehicleModel::lowBeamChanged, this, [this]() { setLowBeam(m_vehicleState->lowBeam()); });
    connect(m_vehicleState, &VehicleModel::positionLampChanged, this, [this]() { setPositionLamp(m_vehicleState->positionLamp()); });
    connect(m_vehicleState, &VehicleModel::lightModeChanged, this, [this]() { setLightMode(m_vehicleState->lightMode()); });

    connect(m_vehicleState, &VehicleModel::doorFrontLeftChanged, this, [this]() { setDoorFrontLeft(m_vehicleState->doorFrontLeft()); });
    connect(m_vehicleState, &VehicleModel::doorFrontRightChanged, this, [this]() { setDoorFrontRight(m_vehicleState->doorFrontRight()); });
    connect(m_vehicleState, &VehicleModel::doorRearLeftChanged, this, [this]() { setDoorRearLeft(m_vehicleState->doorRearLeft()); });
    connect(m_vehicleState, &VehicleModel::doorRearRightChanged, this, [this]() { setDoorRearRight(m_vehicleState->doorRearRight()); });
    connect(m_vehicleState, &VehicleModel::hoodOpenChanged, this, [this]() { setBonnetOpen(m_vehicleState->hoodOpen()); });
    connect(m_vehicleState, &VehicleModel::trunkOpenChanged, this, [this]() { setTrunkOpen(m_vehicleState->trunkOpen()); });

    connect(m_vehicleState, &VehicleModel::cruiseEnabledChanged, this, [this]() { setCruiseEnabled(m_vehicleState->cruiseEnabled()); });
    connect(m_vehicleState, &VehicleModel::rearLeftBuckledChanged, this, [this]() { setRearLeftBuckled(m_vehicleState->rearLeftBuckled()); });
    connect(m_vehicleState, &VehicleModel::rearCenterBuckledChanged, this, [this]() { setRearCenterBuckled(m_vehicleState->rearCenterBuckled()); });
    connect(m_vehicleState, &VehicleModel::rearRightBuckledChanged, this, [this]() { setRearRightBuckled(m_vehicleState->rearRightBuckled()); });
    connect(m_vehicleState, &VehicleModel::rearAlarmActiveChanged, this, [this]() {
        if (m_vehicleState->rearAlarmActive()) {
            if (!m_alerts.rearAlarmActive())
                m_alerts.triggerRearAlarm(m_alerts.rearAlarmSeat() >= 0 ? m_alerts.rearAlarmSeat() : 0);
        } else {
            m_alerts.stopRearAlarm();
        }
    });
    connect(m_vehicleState, &VehicleModel::smartKeyPromptChanged, this, [this]() { setSmartKeyPrompt(m_vehicleState->smartKeyPrompt()); });
    connect(m_vehicleState, &VehicleModel::startPedalPromptChanged, this, [this]() { setStartPedalPrompt(m_vehicleState->startPedalPrompt()); });
    connect(m_vehicleState, &VehicleModel::pressStartAgainAlertChanged, this, [this]() { setPressStartAgainAlert(m_vehicleState->pressStartAgainAlert()); });
    connect(m_vehicleState, &VehicleModel::driverAttentionActiveChanged, this, [this]() { setDriverAttentionActive(m_vehicleState->driverAttentionActive()); });
    connect(m_vehicleState, &VehicleModel::reduceSpeedAlertChanged, this, [this]() { setReduceSpeedAlert(m_vehicleState->reduceSpeedAlert()); });
    connect(m_vehicleState, &VehicleModel::sunroofOpenChanged, this, [this]() { setSunroofOpen(m_vehicleState->sunroofOpen()); });
    connect(m_vehicleState, &VehicleModel::sunroofAlertActiveChanged, this, [this]() { setSunroofAlertActive(m_vehicleState->sunroofAlertActive()); });

    connect(m_vehicleState, &VehicleModel::isMediaPlayingChanged, this, [this]() { setIsMediaPlaying(m_vehicleState->isMediaPlaying()); });
    connect(m_vehicleState, &VehicleModel::mediaSourceChanged, this, [this]() { setMediaSource(m_vehicleState->mediaSource()); });
    connect(m_vehicleState, &VehicleModel::showMediaPopupChanged, this, [this]() { setShowMediaPopup(m_vehicleState->showMediaPopup()); });
    connect(m_vehicleState, &VehicleModel::menuTabChanged, this, [this]() { setMenuTab(m_vehicleState->menuTab()); });
    connect(m_vehicleState, &VehicleModel::tripPageChanged, this, [this]() { setTripPage(m_vehicleState->tripPage()); });

    connect(m_vehicleState, &VehicleModel::mediaNextRequested, this, &ClusterController::nextMediaTrack);
    connect(m_vehicleState, &VehicleModel::mediaPrevRequested, this, &ClusterController::prevMediaTrack);
    connect(m_vehicleState, &VehicleModel::tripResetRequested, this, &ClusterController::resetActiveTripPage);
    connect(m_vehicleState, &VehicleModel::mediaPopupRequested, this, &ClusterController::triggerMediaPopup);
    connect(m_vehicleState, &VehicleModel::infoMenuRequested, this, &ClusterController::triggerInfoMenu);

    connect(m_vehicleState, &VehicleModel::clusterStateChanged, this, [this]() {
        m_sequenceTimer.stop();
        setClusterState(m_vehicleState->clusterState());
    });
    connect(m_vehicleState, &VehicleModel::themeColorChanged, this, [this]() { setThemeColor(m_vehicleState->themeColor()); });

    // Initial sync
    setClusterState(m_vehicleState->clusterState());
    setSpeed(m_vehicleState->speed());
    setRpm(m_vehicleState->rpm());
    setGear(m_vehicleState->gear());
    setFuelLevel(m_vehicleState->fuelLevel());
    setTempLevel(m_vehicleState->coolantTemp());
    setAmbientTemp(m_vehicleState->ambientTemp());
    setOdoKm(m_vehicleState->odoKm());
    setDteKm(m_vehicleState->dteKm());
    setFlPsi(m_vehicleState->flPsi());
    setFrPsi(m_vehicleState->frPsi());
    setRlPsi(m_vehicleState->rlPsi());
    setRrPsi(m_vehicleState->rrPsi());
    setParkBrakeActive(m_vehicleState->parkBrakeActive());
    setSeatbeltActive(m_vehicleState->seatbeltActive());
    setAbsActive(m_vehicleState->absActive());
    setTpmsActive(m_vehicleState->tpmsActive());
    setBatteryActive(m_vehicleState->batteryActive());
    setOilActive(m_vehicleState->oilActive());
    setCheckEngineActive(m_vehicleState->checkEngineActive());
    setEscActive(m_vehicleState->escActive());
    setEscOffActive(m_vehicleState->escOffActive());
    setSteeringActive(m_vehicleState->steeringActive());
    setAirbagActive(m_vehicleState->airbagActive());
    setMasterWarning(m_vehicleState->masterWarning());
    setLightWarning(m_vehicleState->lightWarning());
    setLeftIndicator(m_vehicleState->leftIndicator());
    setRightIndicator(m_vehicleState->rightIndicator());
    setHighBeam(m_vehicleState->highBeam());
    setLowBeam(m_vehicleState->lowBeam());
    setPositionLamp(m_vehicleState->positionLamp());
    setLightMode(m_vehicleState->lightMode());
    setShowLightPopup(false);
    setDoorFrontLeft(m_vehicleState->doorFrontLeft());
    setDoorFrontRight(m_vehicleState->doorFrontRight());
    setDoorRearLeft(m_vehicleState->doorRearLeft());
    setDoorRearRight(m_vehicleState->doorRearRight());
    setBonnetOpen(m_vehicleState->hoodOpen());
    setTrunkOpen(m_vehicleState->trunkOpen());
    setCruiseEnabled(m_vehicleState->cruiseEnabled());
    setRearLeftBuckled(m_vehicleState->rearLeftBuckled());
    setRearCenterBuckled(m_vehicleState->rearCenterBuckled());
    setRearRightBuckled(m_vehicleState->rearRightBuckled());
    if (!m_vehicleState->rearAlarmActive())
        stopRearAlarm();
    setSmartKeyPrompt(m_vehicleState->smartKeyPrompt());
    setStartPedalPrompt(m_vehicleState->startPedalPrompt());
    setPressStartAgainAlert(m_vehicleState->pressStartAgainAlert());
    setDriverAttentionActive(m_vehicleState->driverAttentionActive());
    setReduceSpeedAlert(m_vehicleState->reduceSpeedAlert());
    setSunroofOpen(m_vehicleState->sunroofOpen());
    setSunroofAlertActive(m_vehicleState->sunroofAlertActive());
    setIsMediaPlaying(m_vehicleState->isMediaPlaying());
    setMediaSource(m_vehicleState->mediaSource());
    setShowMediaPopup(m_vehicleState->showMediaPopup());
    setMenuTab(m_vehicleState->menuTab());
    setTripPage(m_vehicleState->tripPage());
    setThemeColor(m_vehicleState->themeColor());
}

void ClusterController::triggerStartupSequence()
{
    if (m_vehicleState) {
        return;
    }

    m_sequenceStepIndex = 1;
    setClusterState(StateInitialStartup);

    resetTrip();
    emit signalStartupAnimationTone();

    setSpeed(0);
    setRpm(0.0);
    setAllTelltales(false);

    m_sequenceTimer.start(3800);
}

void ClusterController::onSequenceStep()
{
    if (m_sequenceStepIndex == 1) {
        m_sequenceStepIndex = 2;
        setClusterState(StateBootCheck);
        emit signalWelcomeChime();
        setBulbCheckTelltales(true);
        m_sequenceTimer.start(5000);
    } else if (m_sequenceStepIndex == 2) {
        m_sequenceStepIndex = 3;
        setClusterState(StateNormalTrip);
        m_telltales.applyNormalTripTelltales();
        m_sequenceTimer.stop();
    }
}

void ClusterController::cycleGear()
{
    if (m_gear == "P") setGear(QString::fromUtf8("R"));
    else if (m_gear == "R") setGear(QString::fromUtf8("N"));
    else if (m_gear == "N") setGear(QString::fromUtf8("D"));
    else if (m_gear == "D" || m_gear == "1" || m_gear == "2" || m_gear == "3" || m_gear == "4" || m_gear == "5") setGear(QString::fromUtf8("M1"));
    else if (m_gear == "M1") setGear(QString::fromUtf8("M2"));
    else if (m_gear == "M2") setGear(QString::fromUtf8("M3"));
    else if (m_gear == "M3") setGear(QString::fromUtf8("M4"));
    else if (m_gear == "M4") setGear(QString::fromUtf8("M5"));
    else setGear(QString::fromUtf8("P"));
}

void ClusterController::driveDemo()
{
    m_driveSim.toggleDemo(*this);
}

void ClusterController::setThemeColor(const QString& color) { if (m_themeColor != color) { m_themeColor = color; emit themeColorChanged(); } }
void ClusterController::setTempUnit(const QString& unit) { if (m_tempUnit != unit) { m_tempUnit = unit; emit tempUnitChanged(); } }
void ClusterController::setFuelUnit(const QString& unit) { if (m_fuelUnit != unit) { m_fuelUnit = unit; emit fuelUnitChanged(); } }
void ClusterController::setTpmsUnit(const QString& unit) { if (m_tpmsUnit != unit) { m_tpmsUnit = unit; emit tpmsUnitChanged(); } }
void ClusterController::setIllumination(int level) { if (m_illumination != level) { m_illumination = level; emit illuminationChanged(); } }
void ClusterController::setLanguage(const QString& lang) { if (m_language != lang) { m_language = lang; emit languageChanged(); } }
void ClusterController::setIsInteractiveMode(bool interactive) { if (m_isInteractiveMode != interactive) { m_isInteractiveMode = interactive; emit isInteractiveModeChanged(); } }

void ClusterController::setClusterState(int state)
{
    if (m_clusterState != state) {
        m_clusterState = state;
        emit clusterStateChanged();
        if (state == StateShutdown) {
            emit signalGoodbyeChime();
        }
    }
}

void ClusterController::setSpeed(int s)
{
    if (m_speed != s) {
        int prevSpeed = m_speed;
        m_speed = s;
        emit speedChanged();

        if (m_speed > 0 && m_clusterState != StateNormalTrip) {
            m_sequenceTimer.stop();
            setClusterState(StateNormalTrip);
        }

        if (!m_vehicleState) {
            m_driveSim.applyManualSpeedAmtCoupling(*this, m_speed);
        }

        m_alerts.evaluateSpeed(prevSpeed, m_speed);
    }
}

void ClusterController::setRpm(double r)
{
    if (std::abs(m_rpm - r) > 0.01) {
        m_rpm = r;
        emit rpmChanged();
    }
}

void ClusterController::setFuelLevel(int f)
{
    if (m_fuelLevel != f) {
        int oldLevel = m_fuelLevel;
        m_fuelLevel = f;
        emit fuelLevelChanged();

        setDteKm(m_trip.calculateDteForFuelLevel(f));

        if (f > oldLevel) {
            resetSinceRefuel();
        }

        if (oldLevel > 2 && f <= 2 && m_clusterState == StateNormalTrip) {
            emit signalWarningChime();
        }
    }
}

void ClusterController::setTempLevel(int t) { if (m_tempLevel != t) { m_tempLevel = t; emit tempLevelChanged(); } }
void ClusterController::setGear(const QString& g) { if (m_gear != g) { m_gear = g; emit gearChanged(); } }
void ClusterController::setOdoKm(int odo) { if (m_odoKm != odo) { m_odoKm = odo; emit odoKmChanged(); } }
void ClusterController::setDteKm(int dte) { if (m_dteKm != dte) { m_dteKm = dte; emit dteKmChanged(); } }
void ClusterController::setAmbientTemp(int temp) { if (m_ambientTemp != temp) { m_ambientTemp = temp; emit ambientTempChanged(); } }

void ClusterController::setMenuTab(int tab)
{
    int t = (tab % 3 + 3) % 3;
    if (m_menuTab != t) {
        m_menuTab = t;
        emit menuTabChanged();
    }
}

void ClusterController::setShowMenuTabs(bool show)
{
    if (m_showMenuTabs != show) {
        m_showMenuTabs = show;
        emit showMenuTabsChanged();
    }
}

void ClusterController::nextMenuTab()
{
    setMenuTab(m_menuTab + 1);
    setShowMenuTabs(true);
}

void ClusterController::triggerInfoMenu()
{
    if (!m_showMenuTabs) {
        setShowMenuTabs(true);
    } else {
        nextMenuTab();
    }
}

// Cruise Control delegations & DRY helper
void ClusterController::applyCruiseTargetSpeed(int target)
{
    setSpeed(target);
    setRpm(1.2 + (target / 100.0) * 1.5);
}

void ClusterController::setCruiseEnabled(bool enabled) { m_alerts.setCruiseEnabled(enabled); }
void ClusterController::setCruiseActive(bool active) { m_alerts.setCruiseActive(active); }
void ClusterController::setCruiseSetSpeed(int speed) { m_alerts.setCruiseSetSpeed(speed); }
void ClusterController::toggleCruise() { m_alerts.toggleCruise(); }

void ClusterController::cruiseSet()
{
    int target = m_alerts.applyCruiseSet(m_speed);
    applyCruiseTargetSpeed(target);
}

void ClusterController::cruiseResPlus()
{
    int target = m_alerts.applyCruiseResPlus();
    applyCruiseTargetSpeed(target);
}

void ClusterController::cruiseSetMinus()
{
    int target = m_alerts.applyCruiseSetMinus(m_speed);
    applyCruiseTargetSpeed(target);
}

void ClusterController::cruiseCancel()
{
    m_alerts.cruiseCancel();
}

// Telltale & Light delegations
void ClusterController::setParkBrakeActive(bool active) { m_telltales.setParkBrakeActive(active); }
void ClusterController::setSeatbeltActive(bool active) { m_telltales.setSeatbeltActive(active); }
void ClusterController::setAbsActive(bool active) { m_telltales.setAbsActive(active); }
void ClusterController::setTpmsActive(bool active) { m_telltales.setTpmsActive(active); }
void ClusterController::setBatteryActive(bool active) { m_telltales.setBatteryActive(active); }
void ClusterController::setOilActive(bool active) { m_telltales.setOilActive(active); }
void ClusterController::setLeftIndicator(bool active) { m_telltales.setLeftIndicator(active); }
void ClusterController::setRightIndicator(bool active) { m_telltales.setRightIndicator(active); }
void ClusterController::setHighBeam(bool active) { m_telltales.setHighBeam(active); }
void ClusterController::setLowBeam(bool active) { m_telltales.setLowBeam(active); }
void ClusterController::setPositionLamp(bool active) { m_telltales.setPositionLamp(active); }
void ClusterController::setMasterWarning(bool active) { m_telltales.setMasterWarning(active); }
void ClusterController::setLightWarning(bool active) { m_telltales.setLightWarning(active); }
void ClusterController::setEscActive(bool active) { m_telltales.setEscActive(active); }
void ClusterController::setEscOffActive(bool active) { m_telltales.setEscOffActive(active); }
void ClusterController::setCheckEngineActive(bool active) { m_telltales.setCheckEngineActive(active); }
void ClusterController::setSmartKeyActive(bool active) { m_telltales.setSmartKeyActive(active); }
void ClusterController::setSteeringActive(bool active) { m_telltales.setSteeringActive(active); }
void ClusterController::setAirbagActive(bool active) { m_telltales.setAirbagActive(active); }
void ClusterController::setAllTelltales(bool active) { m_telltales.setAllTelltales(active); }
void ClusterController::setBulbCheckTelltales(bool active) { m_telltales.setBulbCheckTelltales(active); }
void ClusterController::setLightMode(int mode) { m_telltales.setLightMode(mode); }
void ClusterController::cycleLightMode() { m_telltales.cycleLightMode(); }
void ClusterController::setShowLightPopup(bool show) { m_telltales.setShowLightPopup(show); }

// TPMS delegations
void ClusterController::setFlPsi(double psi) { m_telltales.setFlPsi(psi); }
void ClusterController::setFrPsi(double psi) { m_telltales.setFrPsi(psi); }
void ClusterController::setRlPsi(double psi) { m_telltales.setRlPsi(psi); }
void ClusterController::setRrPsi(double psi) { m_telltales.setRrPsi(psi); }
void ClusterController::setAllPsi(double psi) { m_telltales.setAllPsi(psi); }
void ClusterController::adjustPsi(const QString& tyre, double delta) { m_telltales.adjustPsi(tyre, delta); }
void ClusterController::resetTpms() { m_telltales.resetTpms(); }
void ClusterController::setTpmsCalibrated(bool cal) { m_telltales.setTpmsCalibrated(cal); }

// Trip delegations
void ClusterController::setTripKm(double km) { m_trip.setTripKm(km); }
void ClusterController::setTripTime(const QString& time) { m_trip.setTripTime(time); }
void ClusterController::setTripEconomy(double econ) { m_trip.setTripEconomy(econ); }
void ClusterController::setTripPage(int page) { m_trip.setTripPage(page); }
void ClusterController::setRefuelKm(double km) { m_trip.setRefuelKm(km); }
void ClusterController::setRefuelTime(const QString& time) { m_trip.setRefuelTime(time); }
void ClusterController::setRefuelEconomy(double econ) { m_trip.setRefuelEconomy(econ); }
void ClusterController::setAccumKm(double km) { m_trip.setAccumKm(km); }
void ClusterController::setAccumTime(const QString& time) { m_trip.setAccumTime(time); }
void ClusterController::setAccumEconomy(double econ) { m_trip.setAccumEconomy(econ); }
void ClusterController::setInstantEconomy(double econ) { m_trip.setInstantEconomy(econ); }
void ClusterController::nextTripPage() { m_trip.nextTripPage(); }
void ClusterController::prevTripPage() { m_trip.prevTripPage(); }
void ClusterController::resetTrip() { m_trip.resetTrip(); }
void ClusterController::resetSinceRefuel() { m_trip.resetSinceRefuel(); }
void ClusterController::resetAccumInfo() { m_trip.resetAccumInfo(); }
void ClusterController::resetActiveTripPage() { m_trip.resetActiveTripPage(); }

// Alert & Closure delegations
void ClusterController::setRearLeftBuckled(bool buckled) { m_alerts.setRearLeftBuckled(buckled); }
void ClusterController::setRearCenterBuckled(bool buckled) { m_alerts.setRearCenterBuckled(buckled); }
void ClusterController::setRearRightBuckled(bool buckled) { m_alerts.setRearRightBuckled(buckled); }
void ClusterController::triggerRearAlarm(int seat) { m_alerts.triggerRearAlarm(seat); }
void ClusterController::stopRearAlarm() { m_alerts.stopRearAlarm(); }
void ClusterController::setDriverAttentionActive(bool active) { m_alerts.setDriverAttentionActive(active); }
void ClusterController::setAttentionLevel(int level) { m_alerts.setAttentionLevel(level); }
void ClusterController::triggerDriverAttention() { m_alerts.triggerDriverAttention(); }
void ClusterController::setServiceDueKm(int km) { m_alerts.setServiceDueKm(km); }
void ClusterController::setServiceDueDays(int days) { m_alerts.setServiceDueDays(days); }
void ClusterController::setServicePopupActive(bool active) { m_alerts.setServicePopupActive(active); }
void ClusterController::triggerServiceReminder() { m_alerts.triggerServiceReminder(); }
void ClusterController::resetServiceInterval() { m_alerts.resetServiceInterval(); }
void ClusterController::setSunroofOpen(bool open) { m_alerts.setSunroofOpen(open); }
void ClusterController::setSunroofAlertActive(bool active) { m_alerts.setSunroofAlertActive(active); }
void ClusterController::setSmartKeyPrompt(int prompt) { m_alerts.setSmartKeyPrompt(prompt); }
void ClusterController::showSmartKeyAlert(int type) { m_alerts.showSmartKeyAlert(type); }
void ClusterController::setIsgActive(bool active) { m_alerts.setIsgActive(active); }
void ClusterController::toggleIsg() { m_alerts.toggleIsg(); }
void ClusterController::setReduceSpeedAlert(bool active) { m_alerts.setReduceSpeedAlert(active); }
void ClusterController::triggerReduceSpeedAlert() { m_alerts.triggerReduceSpeedAlert(); }
void ClusterController::setPressStartAgainAlert(bool active) { m_alerts.setPressStartAgainAlert(active); }
void ClusterController::setStartPedalPrompt(int prompt) { m_alerts.setStartPedalPrompt(prompt); }
void ClusterController::setDoorOpenAlert(bool open) { m_alerts.setDoorOpenAlert(open); }
void ClusterController::setDoorOpenSide(int side) { m_alerts.setDoorOpenSide(side); }
void ClusterController::setDoorFrontLeft(bool open) { m_alerts.setDoorFrontLeft(open); }
void ClusterController::setDoorFrontRight(bool open) { m_alerts.setDoorFrontRight(open); }
void ClusterController::setDoorRearLeft(bool open) { m_alerts.setDoorRearLeft(open); }
void ClusterController::setDoorRearRight(bool open) { m_alerts.setDoorRearRight(open); }
void ClusterController::setBonnetOpen(bool open) { m_alerts.setBonnetOpen(open); }
void ClusterController::setTrunkOpen(bool open) { m_alerts.setTrunkOpen(open); }
void ClusterController::setAllDoors(bool open) { m_alerts.setAllDoors(open); }

void ClusterController::triggerShutdown()
{
    setClusterState(StateShutdown);
    setSpeed(0);
    setRpm(0.0);
    setAllTelltales(false);

    emit signalGoodbyeChime();

    if (m_alerts.sunroofOpen()) {
        m_alerts.setSunroofAlertActive(true);
    }

    if (!m_shutdownTimer) {
        m_shutdownTimer = new QTimer(this);
        m_shutdownTimer->setSingleShot(true);
        connect(m_shutdownTimer, &QTimer::timeout, this, &ClusterController::onShutdownTimeout);
    }
    m_shutdownTimer->start(4500);
}

void ClusterController::onShutdownTimeout()
{
    m_alerts.setSunroofAlertActive(false);
    setClusterState(StateOff);
}

// Media delegations
void ClusterController::setShowMediaPopup(bool show) { m_media.setShowMediaPopup(show); }
void ClusterController::setMediaSource(const QString& source) { m_media.setMediaSource(source); }
void ClusterController::setMediaTitle(const QString& title) { m_media.setMediaTitle(title); }
void ClusterController::setMediaArtist(const QString& artist) { m_media.setMediaArtist(artist); }
void ClusterController::setIsMediaPlaying(bool playing) { m_media.setIsMediaPlaying(playing); }
void ClusterController::setMediaTrackIndex(int index) { m_media.setMediaTrackIndex(index); }
void ClusterController::onMediaPopupTimeout() { m_media.dismissMediaPopup(); }
void ClusterController::triggerMediaPopup() { m_media.triggerMediaPopup(); }
void ClusterController::dismissMediaPopup() { m_media.dismissMediaPopup(); }
void ClusterController::playTrack(const QString& source, const QString& artist, const QString& title) { m_media.playTrack(source, artist, title); }
void ClusterController::nextMediaTrack() { m_media.nextMediaTrack(); }
void ClusterController::prevMediaTrack() { m_media.prevMediaTrack(); }
void ClusterController::toggleMediaPlayback() { m_media.toggleMediaPlayback(); }
