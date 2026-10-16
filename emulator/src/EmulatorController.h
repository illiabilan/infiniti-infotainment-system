#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <algorithm>
#include <cmath>
#include "VehicleState.h"

class EmulatorController : public QObject
{
    Q_OBJECT

    // Core telemetry forwarded from VehicleState
    Q_PROPERTY(QString themeColor READ themeColor WRITE setThemeColor NOTIFY themeColorChanged)
    Q_PROPERTY(int clusterState READ clusterState WRITE setClusterState NOTIFY clusterStateChanged)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(double rpm READ rpm WRITE setRpm NOTIFY rpmChanged)
    Q_PROPERTY(QString gear READ gear WRITE setGear NOTIFY gearChanged)
    Q_PROPERTY(int fuelLevel READ fuelLevel WRITE setFuelLevel NOTIFY fuelLevelChanged)
    Q_PROPERTY(int tempLevel READ tempLevel WRITE setTempLevel NOTIFY tempLevelChanged)
    Q_PROPERTY(int ambientTemp READ ambientTemp WRITE setAmbientTemp NOTIFY ambientTempChanged)
    Q_PROPERTY(int odoKm READ odoKm WRITE setOdoKm NOTIFY odoKmChanged)
    Q_PROPERTY(int dteKm READ dteKm WRITE setDteKm NOTIFY dteKmChanged)

    // TPMS
    Q_PROPERTY(double flPsi READ flPsi WRITE setFlPsi NOTIFY flPsiChanged)
    Q_PROPERTY(double frPsi READ frPsi WRITE setFrPsi NOTIFY frPsiChanged)
    Q_PROPERTY(double rlPsi READ rlPsi WRITE setRlPsi NOTIFY rlPsiChanged)
    Q_PROPERTY(double rrPsi READ rrPsi WRITE setRrPsi NOTIFY rrPsiChanged)

    // Telltales & Warnings
    Q_PROPERTY(bool parkBrakeActive READ parkBrakeActive WRITE setParkBrakeActive NOTIFY parkBrakeActiveChanged)
    Q_PROPERTY(bool seatbeltActive READ seatbeltActive WRITE setSeatbeltActive NOTIFY seatbeltActiveChanged)
    Q_PROPERTY(bool absActive READ absActive WRITE setAbsActive NOTIFY absActiveChanged)
    Q_PROPERTY(bool tpmsActive READ tpmsActive WRITE setTpmsActive NOTIFY tpmsActiveChanged)
    Q_PROPERTY(bool batteryActive READ batteryActive WRITE setBatteryActive NOTIFY batteryActiveChanged)
    Q_PROPERTY(bool oilActive READ oilActive WRITE setOilActive NOTIFY oilActiveChanged)
    Q_PROPERTY(bool checkEngineActive READ checkEngineActive WRITE setCheckEngineActive NOTIFY checkEngineActiveChanged)
    Q_PROPERTY(bool escActive READ escActive WRITE setEscActive NOTIFY escActiveChanged)
    Q_PROPERTY(bool escOffActive READ escOffActive WRITE setEscOffActive NOTIFY escOffActiveChanged)
    Q_PROPERTY(bool steeringActive READ steeringActive WRITE setSteeringActive NOTIFY steeringActiveChanged)
    Q_PROPERTY(bool airbagActive READ airbagActive WRITE setAirbagActive NOTIFY airbagActiveChanged)
    Q_PROPERTY(bool masterWarning READ masterWarning WRITE setMasterWarning NOTIFY masterWarningChanged)
    Q_PROPERTY(bool lightWarning READ lightWarning WRITE setLightWarning NOTIFY lightWarningChanged)

    // Lighting
    Q_PROPERTY(bool leftIndicator READ leftIndicator WRITE setLeftIndicator NOTIFY leftIndicatorChanged)
    Q_PROPERTY(bool rightIndicator READ rightIndicator WRITE setRightIndicator NOTIFY rightIndicatorChanged)
    Q_PROPERTY(bool highBeam READ highBeam WRITE setHighBeam NOTIFY highBeamChanged)
    Q_PROPERTY(bool lowBeam READ lowBeam WRITE setLowBeam NOTIFY lowBeamChanged)
    Q_PROPERTY(bool positionLamp READ positionLamp WRITE setPositionLamp NOTIFY positionLampChanged)
    Q_PROPERTY(int lightMode READ lightMode WRITE setLightMode NOTIFY lightModeChanged)

    // Closures
    Q_PROPERTY(bool doorFrontLeft READ doorFrontLeft WRITE setDoorFrontLeft NOTIFY doorFrontLeftChanged)
    Q_PROPERTY(bool doorFrontRight READ doorFrontRight WRITE setDoorFrontRight NOTIFY doorFrontRightChanged)
    Q_PROPERTY(bool doorRearLeft READ doorRearLeft WRITE setDoorRearLeft NOTIFY doorRearLeftChanged)
    Q_PROPERTY(bool doorRearRight READ doorRearRight WRITE setDoorRearRight NOTIFY doorRearRightChanged)
    Q_PROPERTY(bool bonnetOpen READ bonnetOpen WRITE setBonnetOpen NOTIFY bonnetOpenChanged)
    Q_PROPERTY(bool trunkOpen READ trunkOpen WRITE setTrunkOpen NOTIFY trunkOpenChanged)
    Q_PROPERTY(bool centralLock READ centralLock WRITE setCentralLock NOTIFY centralLockChanged)

    // Climate
    Q_PROPERTY(double driverTemp READ driverTemp WRITE setDriverTemp NOTIFY driverTempChanged)
    Q_PROPERTY(double passengerTemp READ passengerTemp WRITE setPassengerTemp NOTIFY passengerTempChanged)
    Q_PROPERTY(int fanSpeed READ fanSpeed WRITE setFanSpeed NOTIFY fanSpeedChanged)
    Q_PROPERTY(int airflowMode READ airflowMode WRITE setAirflowMode NOTIFY airflowModeChanged)
    Q_PROPERTY(bool acActive READ acActive WRITE setAcActive NOTIFY acActiveChanged)
    Q_PROPERTY(bool autoActive READ autoActive WRITE setAutoActive NOTIFY autoActiveChanged)
    Q_PROPERTY(bool dualMode READ dualMode WRITE setDualMode NOTIFY dualModeChanged)
    Q_PROPERTY(bool recircActive READ recircActive WRITE setRecircActive NOTIFY recircActiveChanged)
    Q_PROPERTY(bool frontDefrostActive READ frontDefrostActive WRITE setFrontDefrostActive NOTIFY frontDefrostActiveChanged)
    Q_PROPERTY(bool rearDefrostActive READ rearDefrostActive WRITE setRearDefrostActive NOTIFY rearDefrostActiveChanged)
    Q_PROPERTY(int driverSeatHeat READ driverSeatHeat WRITE setDriverSeatHeat NOTIFY driverSeatHeatChanged)
    Q_PROPERTY(int passengerSeatHeat READ passengerSeatHeat WRITE setPassengerSeatHeat NOTIFY passengerSeatHeatChanged)

    // Cruise & Rear Alarm
    Q_PROPERTY(bool cruiseEnabled READ cruiseEnabled WRITE setCruiseEnabled NOTIFY cruiseEnabledChanged)
    Q_PROPERTY(bool rearLeftBuckled READ rearLeftBuckled WRITE setRearLeftBuckled NOTIFY rearLeftBuckledChanged)
    Q_PROPERTY(bool rearRightBuckled READ rearRightBuckled WRITE setRearRightBuckled NOTIFY rearRightBuckledChanged)
    Q_PROPERTY(bool rearCenterBuckled READ rearCenterBuckled WRITE setRearCenterBuckled NOTIFY rearCenterBuckledChanged)
    Q_PROPERTY(bool rearAlarmActive READ rearAlarmActive WRITE setRearAlarmActive NOTIFY rearAlarmActiveChanged)

    // Alerts & Prompts
    Q_PROPERTY(int smartKeyPrompt READ smartKeyPrompt WRITE setSmartKeyPrompt NOTIFY smartKeyPromptChanged)
    Q_PROPERTY(int startPedalPrompt READ startPedalPrompt WRITE setStartPedalPrompt NOTIFY startPedalPromptChanged)
    Q_PROPERTY(bool pressStartAgainAlert READ pressStartAgainAlert WRITE setPressStartAgainAlert NOTIFY pressStartAgainAlertChanged)
    Q_PROPERTY(bool driverAttentionActive READ driverAttentionActive WRITE setDriverAttentionActive NOTIFY driverAttentionActiveChanged)
    Q_PROPERTY(bool reduceSpeedAlert READ reduceSpeedAlert WRITE setReduceSpeedAlert NOTIFY reduceSpeedAlertChanged)
    Q_PROPERTY(bool sunroofAlertActive READ sunroofAlertActive WRITE setSunroofAlertActive NOTIFY sunroofAlertActiveChanged)

    // Media
    Q_PROPERTY(bool isMediaPlaying READ isMediaPlaying WRITE setIsMediaPlaying NOTIFY isMediaPlayingChanged)
    Q_PROPERTY(QString mediaSource READ mediaSource WRITE setMediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(bool showMediaPopup READ showMediaPopup WRITE setShowMediaPopup NOTIFY showMediaPopupChanged)

    // Simulation / Demo State
    Q_PROPERTY(bool isDemoDriving READ isDemoDriving NOTIFY isDemoDrivingChanged)
    Q_PROPERTY(QString demoScenario READ demoScenario NOTIFY demoScenarioChanged)
    Q_PROPERTY(int menuTab READ menuTab WRITE setMenuTab NOTIFY menuTabChanged)
    Q_PROPERTY(int tripPage READ tripPage WRITE setTripPage NOTIFY tripPageChanged)
    Q_PROPERTY(double tripKm READ tripKm NOTIFY tripKmChanged)
    Q_PROPERTY(double tripEconomy READ tripEconomy NOTIFY tripEconomyChanged)

public:
    explicit EmulatorController(VehicleState *state, QObject *parent = nullptr);

    // Getters
    QString themeColor() const { return m_state->themeColor(); }
    int clusterState() const { return m_state->clusterState(); }
    int speed() const { return m_state->speed(); }
    double rpm() const { return m_state->rpm(); }
    QString gear() const { return m_state->gear(); }
    int fuelLevel() const { return m_state->fuelLevel(); }
    int tempLevel() const { return m_state->coolantTemp(); }
    int ambientTemp() const { return m_state->ambientTemp(); }
    int odoKm() const { return m_state->odoKm(); }
    int dteKm() const { return m_state->dteKm(); }

    double flPsi() const { return m_state->flPsi(); }
    double frPsi() const { return m_state->frPsi(); }
    double rlPsi() const { return m_state->rlPsi(); }
    double rrPsi() const { return m_state->rrPsi(); }

    bool parkBrakeActive() const { return m_state->parkBrakeActive(); }
    bool seatbeltActive() const { return m_state->seatbeltActive(); }
    bool absActive() const { return m_state->absActive(); }
    bool tpmsActive() const { return m_state->tpmsActive(); }
    bool batteryActive() const { return m_state->batteryActive(); }
    bool oilActive() const { return m_state->oilActive(); }
    bool checkEngineActive() const { return m_state->checkEngineActive(); }
    bool escActive() const { return m_state->escActive(); }
    bool escOffActive() const { return m_state->escOffActive(); }
    bool steeringActive() const { return m_state->steeringActive(); }
    bool airbagActive() const { return m_state->airbagActive(); }
    bool masterWarning() const { return m_state->masterWarning(); }
    bool lightWarning() const { return m_state->lightWarning(); }

    bool leftIndicator() const { return m_state->leftIndicator(); }
    bool rightIndicator() const { return m_state->rightIndicator(); }
    bool highBeam() const { return m_state->highBeam(); }
    bool lowBeam() const { return m_state->lowBeam(); }
    bool positionLamp() const { return m_state->positionLamp(); }
    int lightMode() const { return m_state->lightMode(); }

    bool doorFrontLeft() const { return m_state->doorFrontLeft(); }
    bool doorFrontRight() const { return m_state->doorFrontRight(); }
    bool doorRearLeft() const { return m_state->doorRearLeft(); }
    bool doorRearRight() const { return m_state->doorRearRight(); }
    bool bonnetOpen() const { return m_state->hoodOpen(); }
    bool trunkOpen() const { return m_state->trunkOpen(); }
    bool centralLock() const { return m_state->centralLock(); }

    double driverTemp() const { return m_state->driverTemp(); }
    double passengerTemp() const { return m_state->passengerTemp(); }
    int fanSpeed() const { return m_state->fanSpeed(); }
    int airflowMode() const { return m_state->airflowMode(); }
    bool acActive() const { return m_state->acActive(); }
    bool autoActive() const { return m_state->autoActive(); }
    bool dualMode() const { return m_state->dualMode(); }
    bool recircActive() const { return m_state->recircActive(); }
    bool frontDefrostActive() const { return m_state->frontDefrostActive(); }
    bool rearDefrostActive() const { return m_state->rearDefrostActive(); }
    int driverSeatHeat() const { return m_state->driverSeatHeat(); }
    int passengerSeatHeat() const { return m_state->passengerSeatHeat(); }

    bool cruiseEnabled() const { return m_state->cruiseEnabled(); }
    bool rearLeftBuckled() const { return m_state->rearLeftBuckled(); }
    bool rearRightBuckled() const { return m_state->rearRightBuckled(); }
    bool rearCenterBuckled() const { return m_state->rearCenterBuckled(); }
    bool rearAlarmActive() const { return m_state->rearAlarmActive(); }

    int smartKeyPrompt() const { return m_state->smartKeyPrompt(); }
    int startPedalPrompt() const { return m_state->startPedalPrompt(); }
    bool pressStartAgainAlert() const { return m_state->pressStartAgainAlert(); }
    bool driverAttentionActive() const { return m_state->driverAttentionActive(); }
    bool reduceSpeedAlert() const { return m_state->reduceSpeedAlert(); }
    bool sunroofAlertActive() const { return m_state->sunroofAlertActive(); }

    bool isMediaPlaying() const { return m_state->isMediaPlaying(); }
    QString mediaSource() const { return m_state->mediaSource(); }
    bool showMediaPopup() const { return m_state->showMediaPopup(); }

    bool isDemoDriving() const { return m_isDemoDriving; }
    QString demoScenario() const { return m_demoScenario; }
    int menuTab() const { return m_state->menuTab(); }
    int tripPage() const { return m_state->tripPage(); }
    double tripKm() const { return m_tripKm; }
    double tripEconomy() const { return m_tripEconomy; }

public slots:
    void setThemeColor(const QString &c);
    void setClusterState(int s);
    void setSpeed(int s);
    void setRpm(double r);
    void setGear(const QString &g);
    void setFuelLevel(int l);
    void setTempLevel(int t);
    void setAmbientTemp(int t);
    void setOdoKm(int o);
    void setDteKm(int d);

    void setFlPsi(double v);
    void setFrPsi(double v);
    void setRlPsi(double v);
    void setRrPsi(double v);
    void setAllPsi(double psi);

    void setParkBrakeActive(bool b);
    void setSeatbeltActive(bool b);
    void setAbsActive(bool b);
    void setTpmsActive(bool b);
    void setBatteryActive(bool b);
    void setOilActive(bool b);
    void setCheckEngineActive(bool b);
    void setEscActive(bool b);
    void setEscOffActive(bool b);
    void setSteeringActive(bool b);
    void setAirbagActive(bool b);
    void setMasterWarning(bool b);
    void setLightWarning(bool b);
    void setAllTelltales(bool active);

    void setLeftIndicator(bool b);
    void setRightIndicator(bool b);
    void setHighBeam(bool b);
    void setLowBeam(bool b);
    void setPositionLamp(bool b);
    void setLightMode(int m);

    void setDoorFrontLeft(bool b);
    void setDoorFrontRight(bool b);
    void setDoorRearLeft(bool b);
    void setDoorRearRight(bool b);
    void setBonnetOpen(bool b);
    void setTrunkOpen(bool b);
    void setCentralLock(bool b);
    void setAllDoors(bool open);
    void toggleDoorFL();
    void toggleDoorFR();
    void toggleDoorRL();
    void toggleDoorRR();

    void setDriverTemp(double t);
    void setPassengerTemp(double t);
    void setFanSpeed(int s);
    void setAirflowMode(int m);
    void setAcActive(bool b);
    void setAutoActive(bool b);
    void setDualMode(bool b);
    void setRecircActive(bool b);
    void setFrontDefrostActive(bool b);
    void setRearDefrostActive(bool b);
    void setDriverSeatHeat(int h);
    void setPassengerSeatHeat(int h);

    void setCruiseEnabled(bool b);
    void setRearLeftBuckled(bool b);
    void setRearRightBuckled(bool b);
    void setRearCenterBuckled(bool b);
    void setRearAlarmActive(bool b);
    void stopRearAlarm();

    void setSmartKeyPrompt(int p);
    void showSmartKeyAlert(int type);
    void setStartPedalPrompt(int p);
    void showStartPedalAlert(int prompt);
    void setPressStartAgainAlert(bool b);
    void setDriverAttentionActive(bool b);
    void setReduceSpeedAlert(bool b);
    void setSunroofAlertActive(bool b);

    void setIsMediaPlaying(bool b);
    void setMediaSource(const QString &s);
    void setShowMediaPopup(bool b);
    void toggleMediaPlayback();
    void nextMediaTrack();
    void prevMediaTrack();
    void triggerMediaPopup();

    void setMenuTab(int t);
    void setTripPage(int p);
    void resetActiveTripPage();

    // Tire actions
    void deflateFL();
    void deflateFR();
    void deflateRL();
    void deflateRR();
    void punctureFL();
    void punctureFR();
    void punctureRL();
    void punctureRR();
    void setAllTiresOK();

    // Cruise actions
    void toggleCruise();
    void cruiseCancel();
    void cruiseResPlus();
    void cruiseSetMinus();

    // Engine & sequence actions
    void setIgnitionOnDirect();
    void setIgnitionOffDirect();
    void driveDemo();
    void triggerStartupSequence();
    void triggerShutdown();
    void cycleGear();
    void cycleLightMode();
    void resetTrip();
    void nextTripPage();
    void prevTripPage();
    void triggerInfoMenu();

private slots:
    void onDriveSimulationTick();

signals:
    void themeColorChanged();
    void clusterStateChanged();
    void speedChanged();
    void rpmChanged();
    void gearChanged();
    void fuelLevelChanged();
    void tempLevelChanged();
    void ambientTempChanged();
    void odoKmChanged();
    void dteKmChanged();

    void flPsiChanged();
    void frPsiChanged();
    void rlPsiChanged();
    void rrPsiChanged();

    void parkBrakeActiveChanged();
    void seatbeltActiveChanged();
    void absActiveChanged();
    void tpmsActiveChanged();
    void batteryActiveChanged();
    void oilActiveChanged();
    void checkEngineActiveChanged();
    void escActiveChanged();
    void escOffActiveChanged();
    void steeringActiveChanged();
    void airbagActiveChanged();
    void masterWarningChanged();
    void lightWarningChanged();

    void leftIndicatorChanged();
    void rightIndicatorChanged();
    void highBeamChanged();
    void lowBeamChanged();
    void positionLampChanged();
    void lightModeChanged();

    void doorFrontLeftChanged();
    void doorFrontRightChanged();
    void doorRearLeftChanged();
    void doorRearRightChanged();
    void bonnetOpenChanged();
    void trunkOpenChanged();
    void centralLockChanged();

    void driverTempChanged();
    void passengerTempChanged();
    void fanSpeedChanged();
    void airflowModeChanged();
    void acActiveChanged();
    void autoActiveChanged();
    void dualModeChanged();
    void recircActiveChanged();
    void frontDefrostActiveChanged();
    void rearDefrostActiveChanged();
    void driverSeatHeatChanged();
    void passengerSeatHeatChanged();

    void cruiseEnabledChanged();
    void rearLeftBuckledChanged();
    void rearRightBuckledChanged();
    void rearCenterBuckledChanged();
    void rearAlarmActiveChanged();

    void smartKeyPromptChanged();
    void startPedalPromptChanged();
    void pressStartAgainAlertChanged();
    void driverAttentionActiveChanged();
    void reduceSpeedAlertChanged();
    void sunroofAlertActiveChanged();

    void isMediaPlayingChanged();
    void mediaSourceChanged();
    void showMediaPopupChanged();

    void isDemoDrivingChanged();
    void demoScenarioChanged();
    void menuTabChanged();
    void tripPageChanged();
    void tripKmChanged();
    void tripEconomyChanged();

private:
    VehicleState *m_state{nullptr};
    QTimer m_driveSimTimer;
    bool m_isDemoDriving{false};
    QString m_demoScenario{"Idle / Parked"};
    double m_demoCycleTime{0.0};
    int m_demoPrevGear{1};
    double m_tripKm{12.4};
    double m_tripEconomy{10.2};
};
