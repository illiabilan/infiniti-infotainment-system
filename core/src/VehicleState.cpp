#include "VehicleState.h"
#include "NissanCanDefinitions.h"
#include "CanInterface.h"

static VehicleState *s_liveInstance = nullptr;

VehicleState::VehicleState(QObject *parent)
    : VehicleModel(parent)
{
    if (s_liveInstance) {
        qFatal("VehicleState: second instance in one process. Only the Core process owns VehicleState (CL-1).");
    }
    s_liveInstance = this;
}

VehicleState::~VehicleState()
{
    if (s_liveInstance == this) {
        s_liveInstance = nullptr;
    }
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

// Outbound frames
void VehicleState::publish(VehicleModel::Group group, quint8 commandAction)
{
    if (!m_canInterface) return;

    CanFrame f;
    switch (group) {
    case Group::Engine:
        f = NissanCan::encodeEngine(rpm() <= 16.0 ? rpm() * 1000.0 : rpm(), throttlePercent(), engineRunning());
        break;
    case Group::SpeedBrake:
        f = NissanCan::encodeSpeedBrake(speed(), brakePressed(), parkBrakeActive());
        break;
    case Group::Gear:
        f = NissanCan::encodeGear(gear().toStdString());
        break;
    case Group::CoolantFuel:
        f = NissanCan::encodeCoolantFuel(coolantTemp(), fuelLevel(), ambientTemp(), odoKm(), dteKm());
        break;
    case Group::Tpms:
        f = NissanCan::encodeTpms(flPsi(), frPsi(), rlPsi(), rrPsi());
        break;
    case Group::BodyClosures:
        f = NissanCan::encodeBodyClosures(doorFrontLeft(), doorFrontRight(), doorRearLeft(), doorRearRight(),
                                          hoodOpen(), trunkOpen(), centralLock(),
                                          seatbeltActive(), passSeatbeltActive());
        break;
    case Group::Lighting:
        f = NissanCan::encodeLighting(leftIndicator(), rightIndicator(),
                                      lowBeam(), highBeam(), positionLamp(), lightMode());
        break;
    case Group::Telltales:
        f = NissanCan::encodeTelltales(absActive(), airbagActive(), batteryActive(), oilActive(),
                                       checkEngineActive(), escActive(), escOffActive(),
                                       steeringActive(), masterWarning(),
                                       lightWarning(), tpmsActive());
        break;
    case Group::OccupantsAlerts:
        f = NissanCan::encodeOccupantsAlerts(cruiseEnabled(), rearLeftBuckled(), rearCenterBuckled(), rearRightBuckled(),
                                             rearAlarmActive(), smartKeyPrompt(), startPedalPrompt(),
                                             pressStartAgainAlert(), driverAttentionActive(), reduceSpeedAlert(),
                                             sunroofOpen(), sunroofAlertActive());
        break;
    case Group::InfotainmentMfd:
        f = NissanCan::encodeInfotainmentMfd(isMediaPlaying(), mediaSource().toStdString(), showMediaPopup(),
                                             menuTab(), tripPage(), commandAction);
        break;
    case Group::Climate:
        f = NissanCan::encodeClimate(driverTemp(), passengerTemp(), fanSpeed(), airflowMode(),
                                     acActive(), autoActive(), dualMode(), recircActive(),
                                     frontDefrostActive(), rearDefrostActive(),
                                     driverSeatHeat(), passengerSeatHeat(), ecoMode());
        break;
    case Group::ModeTheme:
        f = NissanCan::encodeModeTheme(clusterState(), themeColor().toStdString());
        break;
    }
    m_canInterface->sendFrame(f);
}
