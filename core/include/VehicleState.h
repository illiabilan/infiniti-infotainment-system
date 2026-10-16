#pragma once

#include "CanFrame.h"
#include <QObject>
#include <QString>

class CanInterface;

class VehicleState : public QObject
{
    Q_OBJECT

    // Powertrain & Driving
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(double rpm READ rpm WRITE setRpm NOTIFY rpmChanged)
    Q_PROPERTY(QString gear READ gear WRITE setGear NOTIFY gearChanged)
    Q_PROPERTY(int fuelLevel READ fuelLevel WRITE setFuelLevel NOTIFY fuelLevelChanged)
    Q_PROPERTY(int coolantTemp READ coolantTemp WRITE setCoolantTemp NOTIFY coolantTempChanged)
    Q_PROPERTY(int ambientTemp READ ambientTemp WRITE setAmbientTemp NOTIFY ambientTempChanged)
    Q_PROPERTY(int outsideTemp READ ambientTemp WRITE setAmbientTemp NOTIFY ambientTempChanged)
    Q_PROPERTY(int odoKm READ odoKm WRITE setOdoKm NOTIFY odoKmChanged)
    Q_PROPERTY(int dteKm READ dteKm WRITE setDteKm NOTIFY dteKmChanged)
    Q_PROPERTY(double throttlePercent READ throttlePercent WRITE setThrottlePercent NOTIFY throttlePercentChanged)
    Q_PROPERTY(bool engineRunning READ engineRunning WRITE setEngineRunning NOTIFY engineRunningChanged)

    // Chassis & Safety
    Q_PROPERTY(double flPsi READ flPsi WRITE setFlPsi NOTIFY flPsiChanged)
    Q_PROPERTY(double frPsi READ frPsi WRITE setFrPsi NOTIFY frPsiChanged)
    Q_PROPERTY(double rlPsi READ rlPsi WRITE setRlPsi NOTIFY rlPsiChanged)
    Q_PROPERTY(double rrPsi READ rrPsi WRITE setRrPsi NOTIFY rrPsiChanged)
    Q_PROPERTY(bool brakePressed READ brakePressed WRITE setBrakePressed NOTIFY brakePressedChanged)
    Q_PROPERTY(bool parkBrakeActive READ parkBrakeActive WRITE setParkBrakeActive NOTIFY parkBrakeActiveChanged)
    Q_PROPERTY(bool seatbeltActive READ seatbeltActive WRITE setSeatbeltActive NOTIFY seatbeltActiveChanged)
    Q_PROPERTY(bool passSeatbeltActive READ passSeatbeltActive WRITE setPassSeatbeltActive NOTIFY passSeatbeltActiveChanged)
    Q_PROPERTY(bool absActive READ absActive WRITE setAbsActive NOTIFY absActiveChanged)
    Q_PROPERTY(bool batteryActive READ batteryActive WRITE setBatteryActive NOTIFY batteryActiveChanged)
    Q_PROPERTY(bool oilActive READ oilActive WRITE setOilActive NOTIFY oilActiveChanged)
    Q_PROPERTY(bool checkEngineActive READ checkEngineActive WRITE setCheckEngineActive NOTIFY checkEngineActiveChanged)
    Q_PROPERTY(bool escActive READ escActive WRITE setEscActive NOTIFY escActiveChanged)
    Q_PROPERTY(bool escOffActive READ escOffActive WRITE setEscOffActive NOTIFY escOffActiveChanged)
    Q_PROPERTY(bool steeringActive READ steeringActive WRITE setSteeringActive NOTIFY steeringActiveChanged)
    Q_PROPERTY(bool airbagActive READ airbagActive WRITE setAirbagActive NOTIFY airbagActiveChanged)
    Q_PROPERTY(bool tpmsActive READ tpmsActive WRITE setTpmsActive NOTIFY tpmsActiveChanged)
    Q_PROPERTY(bool masterWarning READ masterWarning WRITE setMasterWarning NOTIFY masterWarningChanged)
    Q_PROPERTY(bool lightWarning READ lightWarning WRITE setLightWarning NOTIFY lightWarningChanged)

    // Body & Access
    Q_PROPERTY(bool doorFrontLeft READ doorFrontLeft WRITE setDoorFrontLeft NOTIFY doorFrontLeftChanged)
    Q_PROPERTY(bool doorFrontRight READ doorFrontRight WRITE setDoorFrontRight NOTIFY doorFrontRightChanged)
    Q_PROPERTY(bool doorRearLeft READ doorRearLeft WRITE setDoorRearLeft NOTIFY doorRearLeftChanged)
    Q_PROPERTY(bool doorRearRight READ doorRearRight WRITE setDoorRearRight NOTIFY doorRearRightChanged)
    Q_PROPERTY(bool hoodOpen READ hoodOpen WRITE setHoodOpen NOTIFY hoodOpenChanged)
    Q_PROPERTY(bool trunkOpen READ trunkOpen WRITE setTrunkOpen NOTIFY trunkOpenChanged)
    Q_PROPERTY(bool centralLock READ centralLock WRITE setCentralLock NOTIFY centralLockChanged)
    Q_PROPERTY(bool sunroofOpen READ sunroofOpen WRITE setSunroofOpen NOTIFY sunroofOpenChanged)
    Q_PROPERTY(bool isAnyDoorOpen READ isAnyDoorOpen NOTIFY isAnyDoorOpenChanged)

    //Occupants, Prompts & Alerts (0x652)
    Q_PROPERTY(bool cruiseEnabled READ cruiseEnabled WRITE setCruiseEnabled NOTIFY cruiseEnabledChanged)
    Q_PROPERTY(bool rearLeftBuckled READ rearLeftBuckled WRITE setRearLeftBuckled NOTIFY rearLeftBuckledChanged)
    Q_PROPERTY(bool rearCenterBuckled READ rearCenterBuckled WRITE setRearCenterBuckled NOTIFY rearCenterBuckledChanged)
    Q_PROPERTY(bool rearRightBuckled READ rearRightBuckled WRITE setRearRightBuckled NOTIFY rearRightBuckledChanged)
    Q_PROPERTY(bool rearAlarmActive READ rearAlarmActive WRITE setRearAlarmActive NOTIFY rearAlarmActiveChanged)
    Q_PROPERTY(int smartKeyPrompt READ smartKeyPrompt WRITE setSmartKeyPrompt NOTIFY smartKeyPromptChanged)
    Q_PROPERTY(int startPedalPrompt READ startPedalPrompt WRITE setStartPedalPrompt NOTIFY startPedalPromptChanged)
    Q_PROPERTY(bool pressStartAgainAlert READ pressStartAgainAlert WRITE setPressStartAgainAlert NOTIFY pressStartAgainAlertChanged)
    Q_PROPERTY(bool driverAttentionActive READ driverAttentionActive WRITE setDriverAttentionActive NOTIFY driverAttentionActiveChanged)
    Q_PROPERTY(bool reduceSpeedAlert READ reduceSpeedAlert WRITE setReduceSpeedAlert NOTIFY reduceSpeedAlertChanged)
    Q_PROPERTY(bool sunroofAlertActive READ sunroofAlertActive WRITE setSunroofAlertActive NOTIFY sunroofAlertActiveChanged)

    // Infotainment & MFD Navigation (0x653)
    Q_PROPERTY(bool isMediaPlaying READ isMediaPlaying WRITE setIsMediaPlaying NOTIFY isMediaPlayingChanged)
    Q_PROPERTY(QString mediaSource READ mediaSource WRITE setMediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(bool showMediaPopup READ showMediaPopup WRITE setShowMediaPopup NOTIFY showMediaPopupChanged)
    Q_PROPERTY(int menuTab READ menuTab WRITE setMenuTab NOTIFY menuTabChanged)
    Q_PROPERTY(int tripPage READ tripPage WRITE setTripPage NOTIFY tripPageChanged)

    // Lighting
    Q_PROPERTY(bool leftIndicator READ leftIndicator WRITE setLeftIndicator NOTIFY leftIndicatorChanged)
    Q_PROPERTY(bool rightIndicator READ rightIndicator WRITE setRightIndicator NOTIFY rightIndicatorChanged)
    Q_PROPERTY(bool lowBeam READ lowBeam WRITE setLowBeam NOTIFY lowBeamChanged)
    Q_PROPERTY(bool highBeam READ highBeam WRITE setHighBeam NOTIFY highBeamChanged)
    Q_PROPERTY(bool positionLamp READ positionLamp WRITE setPositionLamp NOTIFY positionLampChanged)
    Q_PROPERTY(int lightMode READ lightMode WRITE setLightMode NOTIFY lightModeChanged)

    // Climate / HVAC
    Q_PROPERTY(double driverTemp READ driverTemp WRITE setDriverTemp NOTIFY driverTempChanged)
    Q_PROPERTY(double passengerTemp READ passengerTemp WRITE setPassengerTemp NOTIFY passengerTempChanged)
    Q_PROPERTY(int fanSpeed READ fanSpeed WRITE setFanSpeed NOTIFY fanSpeedChanged)
    Q_PROPERTY(int airflowMode READ airflowMode WRITE setAirflowMode NOTIFY airflowModeChanged)
    Q_PROPERTY(bool acActive READ acActive WRITE setAcActive NOTIFY acActiveChanged)
    Q_PROPERTY(bool autoActive READ autoActive WRITE setAutoActive NOTIFY autoActiveChanged)
    Q_PROPERTY(bool dualMode READ dualMode WRITE setDualMode NOTIFY dualModeChanged)
    Q_PROPERTY(bool recircActive READ recircActive WRITE setRecircActive NOTIFY recircActiveChanged)
    Q_PROPERTY(bool ecoMode READ ecoMode WRITE setEcoMode NOTIFY ecoModeChanged)
    Q_PROPERTY(bool frontDefrostActive READ frontDefrostActive WRITE setFrontDefrostActive NOTIFY frontDefrostActiveChanged)
    Q_PROPERTY(bool rearDefrostActive READ rearDefrostActive WRITE setRearDefrostActive NOTIFY rearDefrostActiveChanged)
    Q_PROPERTY(int driverSeatHeat READ driverSeatHeat WRITE setDriverSeatHeat NOTIFY driverSeatHeatChanged)
    Q_PROPERTY(int passengerSeatHeat READ passengerSeatHeat WRITE setPassengerSeatHeat NOTIFY passengerSeatHeatChanged)

    // Vehicle System / Theme
    Q_PROPERTY(int clusterState READ clusterState WRITE setClusterState NOTIFY clusterStateChanged)
    Q_PROPERTY(QString themeColor READ themeColor WRITE setThemeColor NOTIFY themeColorChanged)

public:
    explicit VehicleState(QObject *parent = nullptr);
    static VehicleState* instance();

    void attachCanInterface(CanInterface *canInterface);
    CanInterface* canInterface() const { return m_canInterface; }

    // Powertrain getters
    int speed() const { return m_speed; }
    double rpm() const { return m_rpm; }
    QString gear() const { return m_gear; }
    int fuelLevel() const { return m_fuelLevel; }
    int coolantTemp() const { return m_coolantTemp; }
    int ambientTemp() const { return m_ambientTemp; }
    int odoKm() const { return m_odoKm; }
    int dteKm() const { return m_dteKm; }
    double throttlePercent() const { return m_throttlePercent; }
    bool engineRunning() const { return m_engineRunning; }

    // Chassis getters
    double flPsi() const { return m_flPsi; }
    double frPsi() const { return m_frPsi; }
    double rlPsi() const { return m_rlPsi; }
    double rrPsi() const { return m_rrPsi; }
    bool brakePressed() const { return m_brakePressed; }
    bool parkBrakeActive() const { return m_parkBrakeActive; }
    bool seatbeltActive() const { return m_seatbeltActive; }
    bool passSeatbeltActive() const { return m_passSeatbeltActive; }
    bool absActive() const { return m_absActive; }
    bool batteryActive() const { return m_batteryActive; }
    bool oilActive() const { return m_oilActive; }
    bool checkEngineActive() const { return m_checkEngineActive; }
    bool escActive() const { return m_escActive; }
    bool escOffActive() const { return m_escOffActive; }
    bool steeringActive() const { return m_steeringActive; }
    bool airbagActive() const { return m_airbagActive; }
    bool tpmsActive() const { return m_tpmsActive; }
    bool masterWarning() const { return m_masterWarning; }
    bool lightWarning() const { return m_lightWarning; }

    // Body getters
    bool doorFrontLeft() const { return m_doorFrontLeft; }
    bool doorFrontRight() const { return m_doorFrontRight; }
    bool doorRearLeft() const { return m_doorRearLeft; }
    bool doorRearRight() const { return m_doorRearRight; }
    bool hoodOpen() const { return m_hoodOpen; }
    bool trunkOpen() const { return m_trunkOpen; }
    bool centralLock() const { return m_centralLock; }
    bool sunroofOpen() const { return m_sunroofOpen; }
    bool isAnyDoorOpen() const;

    // Occupants, Prompts & Alerts getters
    bool cruiseEnabled() const { return m_cruiseEnabled; }
    bool rearLeftBuckled() const { return m_rearLeftBuckled; }
    bool rearCenterBuckled() const { return m_rearCenterBuckled; }
    bool rearRightBuckled() const { return m_rearRightBuckled; }
    bool rearAlarmActive() const { return m_rearAlarmActive; }
    int smartKeyPrompt() const { return m_smartKeyPrompt; }
    int startPedalPrompt() const { return m_startPedalPrompt; }
    bool pressStartAgainAlert() const { return m_pressStartAgainAlert; }
    bool driverAttentionActive() const { return m_driverAttentionActive; }
    bool reduceSpeedAlert() const { return m_reduceSpeedAlert; }
    bool sunroofAlertActive() const { return m_sunroofAlertActive; }

    // Infotainment & MFD getters
    bool isMediaPlaying() const { return m_isMediaPlaying; }
    QString mediaSource() const { return m_mediaSource; }
    bool showMediaPopup() const { return m_showMediaPopup; }
    int menuTab() const { return m_menuTab; }
    int tripPage() const { return m_tripPage; }

    // Lighting getters
    bool leftIndicator() const { return m_leftIndicator; }
    bool rightIndicator() const { return m_rightIndicator; }
    bool lowBeam() const { return m_lowBeam; }
    bool highBeam() const { return m_highBeam; }
    bool positionLamp() const { return m_positionLamp; }
    int lightMode() const { return m_lightMode; }

    // Climate getters
    double driverTemp() const { return m_driverTemp; }
    double passengerTemp() const { return m_passengerTemp; }
    int fanSpeed() const { return m_fanSpeed; }
    int airflowMode() const { return m_airflowMode; }
    bool acActive() const { return m_acActive; }
    bool autoActive() const { return m_autoActive; }
    bool dualMode() const { return m_dualMode; }
    bool recircActive() const { return m_recircActive; }
    bool ecoMode() const { return m_ecoMode; }
    bool frontDefrostActive() const { return m_frontDefrostActive; }
    bool rearDefrostActive() const { return m_rearDefrostActive; }
    int driverSeatHeat() const { return m_driverSeatHeat; }
    int passengerSeatHeat() const { return m_passengerSeatHeat; }

    // System getters
    int clusterState() const { return m_clusterState; }
    QString themeColor() const { return m_themeColor; }

public slots:
    void processFrame(const CanFrame &frame);

    // Setters
    void setSpeed(int speed);
    void setRpm(double rpm);
    void setGear(const QString &gear);
    void setFuelLevel(int level);
    void setCoolantTemp(int temp);
    void setAmbientTemp(int temp);
    void setOdoKm(int odo);
    void setDteKm(int dte);
    void setThrottlePercent(double throttle);
    void setEngineRunning(bool running);

    void setFlPsi(double val);
    void setFrPsi(double val);
    void setRlPsi(double val);
    void setRrPsi(double val);
    void setBrakePressed(bool val);
    void setParkBrakeActive(bool val);
    void setSeatbeltActive(bool val);
    void setPassSeatbeltActive(bool val);
    void setAbsActive(bool val);
    void setBatteryActive(bool val);
    void setOilActive(bool val);
    void setCheckEngineActive(bool val);
    void setEscActive(bool val);
    void setEscOffActive(bool val);
    void setSteeringActive(bool val);
    void setAirbagActive(bool val);
    void setTpmsActive(bool val);
    void setMasterWarning(bool val);
    void setLightWarning(bool val);

    void setDoorFrontLeft(bool val);
    void setDoorFrontRight(bool val);
    void setDoorRearLeft(bool val);
    void setDoorRearRight(bool val);
    void setHoodOpen(bool val);
    void setTrunkOpen(bool val);
    void setCentralLock(bool val);
    void setSunroofOpen(bool val);

    void setCruiseEnabled(bool val);
    void setRearLeftBuckled(bool val);
    void setRearCenterBuckled(bool val);
    void setRearRightBuckled(bool val);
    void setRearAlarmActive(bool val);
    void setSmartKeyPrompt(int val);
    void setStartPedalPrompt(int val);
    void setPressStartAgainAlert(bool val);
    void setDriverAttentionActive(bool val);
    void setReduceSpeedAlert(bool val);
    void setSunroofAlertActive(bool val);

    void setIsMediaPlaying(bool val);
    void setMediaSource(const QString &val);
    void setShowMediaPopup(bool val);
    void setMenuTab(int val);
    void setTripPage(int val);

    void setLeftIndicator(bool val);
    void setRightIndicator(bool val);
    void setLowBeam(bool val);
    void setHighBeam(bool val);
    void setPositionLamp(bool val);
    void setLightMode(int val);

    void setDriverTemp(double val);
    void setPassengerTemp(double val);
    void setFanSpeed(int val);
    void setAirflowMode(int val);
    void setAcActive(bool val);
    void setAutoActive(bool val);
    void setDualMode(bool val);
    void setRecircActive(bool val);
    void setEcoMode(bool val);
    void setFrontDefrostActive(bool val);
    void setRearDefrostActive(bool val);
    void setDriverSeatHeat(int val);
    void setPassengerSeatHeat(int val);

    void setClusterState(int state);
    void setThemeColor(const QString &color);

    // Outbound broadcast helpers
    void broadcastEngine();
    void broadcastSpeedBrake();
    void broadcastGear();
    void broadcastCoolantFuel();
    void broadcastTpms();
    void broadcastBodyClosures();
    void broadcastLighting();
    void broadcastTelltales();
    void broadcastOccupantsAlerts();
    void broadcastInfotainmentMfd(uint8_t commandAction = 0);
    void broadcastClimate();
    void broadcastModeTheme();
    void broadcastAll();

signals:
    void speedChanged();
    void rpmChanged();
    void gearChanged();
    void fuelLevelChanged();
    void coolantTempChanged();
    void ambientTempChanged();
    void odoKmChanged();
    void dteKmChanged();
    void throttlePercentChanged();
    void engineRunningChanged();

    void flPsiChanged();
    void frPsiChanged();
    void rlPsiChanged();
    void rrPsiChanged();
    void brakePressedChanged();
    void parkBrakeActiveChanged();
    void seatbeltActiveChanged();
    void passSeatbeltActiveChanged();
    void absActiveChanged();
    void batteryActiveChanged();
    void oilActiveChanged();
    void checkEngineActiveChanged();
    void escActiveChanged();
    void escOffActiveChanged();
    void steeringActiveChanged();
    void airbagActiveChanged();
    void tpmsActiveChanged();
    void masterWarningChanged();
    void lightWarningChanged();

    void doorFrontLeftChanged();
    void doorFrontRightChanged();
    void doorRearLeftChanged();
    void doorRearRightChanged();
    void hoodOpenChanged();
    void trunkOpenChanged();
    void centralLockChanged();
    void sunroofOpenChanged();
    void isAnyDoorOpenChanged();

    void cruiseEnabledChanged();
    void rearLeftBuckledChanged();
    void rearCenterBuckledChanged();
    void rearRightBuckledChanged();
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
    void menuTabChanged();
    void tripPageChanged();
    void mediaNextRequested();
    void mediaPrevRequested();
    void tripResetRequested();
    void mediaPopupRequested();
    void infoMenuRequested();

    void leftIndicatorChanged();
    void rightIndicatorChanged();
    void lowBeamChanged();
    void highBeamChanged();
    void positionLampChanged();
    void lightModeChanged();

    void driverTempChanged();
    void passengerTempChanged();
    void fanSpeedChanged();
    void airflowModeChanged();
    void acActiveChanged();
    void autoActiveChanged();
    void dualModeChanged();
    void recircActiveChanged();
    void ecoModeChanged();
    void frontDefrostActiveChanged();
    void rearDefrostActiveChanged();
    void driverSeatHeatChanged();
    void passengerSeatHeatChanged();

    void clusterStateChanged();
    void themeColorChanged();

private:
    CanInterface *m_canInterface{nullptr};

    // Powertrain
    int m_speed{0};
    double m_rpm{0.0};
    QString m_gear{"P"};
    int m_fuelLevel{9};
    int m_coolantTemp{6};
    int m_ambientTemp{24};
    int m_odoKm{142850};
    int m_dteKm{460};
    double m_throttlePercent{0.0};
    bool m_engineRunning{false};

    // Chassis & Safety
    double m_flPsi{35.0};
    double m_frPsi{35.0};
    double m_rlPsi{35.0};
    double m_rrPsi{35.0};
    bool m_brakePressed{false};
    bool m_parkBrakeActive{true};
    bool m_seatbeltActive{false};
    bool m_passSeatbeltActive{false};
    bool m_absActive{false};
    bool m_batteryActive{false};
    bool m_oilActive{false};
    bool m_checkEngineActive{false};
    bool m_escActive{false};
    bool m_escOffActive{false};
    bool m_steeringActive{false};
    bool m_airbagActive{false};
    bool m_tpmsActive{false};
    bool m_masterWarning{false};
    bool m_lightWarning{false};

    // Body & Access
    bool m_doorFrontLeft{false};
    bool m_doorFrontRight{false};
    bool m_doorRearLeft{false};
    bool m_doorRearRight{false};
    bool m_hoodOpen{false};
    bool m_trunkOpen{false};
    bool m_centralLock{true};
    bool m_sunroofOpen{false};

    // Occupants, Prompts & Alerts
    bool m_cruiseEnabled{false};
    bool m_rearLeftBuckled{false};
    bool m_rearCenterBuckled{false};
    bool m_rearRightBuckled{false};
    bool m_rearAlarmActive{false};
    int m_smartKeyPrompt{0};
    int m_startPedalPrompt{0};
    bool m_pressStartAgainAlert{false};
    bool m_driverAttentionActive{false};
    bool m_reduceSpeedAlert{false};
    bool m_sunroofAlertActive{false};

    // Infotainment & MFD
    bool m_isMediaPlaying{true};
    QString m_mediaSource{"Bluetooth"};
    bool m_showMediaPopup{false};
    int m_menuTab{0};
    int m_tripPage{0};

    // Lighting
    bool m_leftIndicator{false};
    bool m_rightIndicator{false};
    bool m_lowBeam{false};
    bool m_highBeam{false};
    bool m_positionLamp{false};
    int m_lightMode{1}; // 1 = Auto

    // Climate
    double m_driverTemp{21.5};
    double m_passengerTemp{22.0};
    int m_fanSpeed{3};
    int m_airflowMode{0}; // 0 = Face
    bool m_acActive{true};
    bool m_autoActive{true};
    bool m_dualMode{true};
    bool m_recircActive{false};
    bool m_ecoMode{false};
    bool m_frontDefrostActive{false};
    bool m_rearDefrostActive{false};
    int m_driverSeatHeat{0};
    int m_passengerSeatHeat{0};

    // System
    int m_clusterState{3}; // 3 = NormalTrip
    QString m_themeColor{"blue"};
};
