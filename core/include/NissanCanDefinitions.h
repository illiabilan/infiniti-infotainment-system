#pragma once

#include "CanFrame.h"
#include <cstdint>
#include <string>

namespace NissanCan {

// Standard Nissan FM Platform CAN Arbitration IDs (500 kbps)
constexpr uint32_t ID_ENGINE_RPM_THROTTLE  = 0x1F9;
constexpr uint32_t ID_VEHICLE_SPEED_BRAKE  = 0x280;
constexpr uint32_t ID_TRANSMISSION_GEAR    = 0x421;
constexpr uint32_t ID_COOLANT_TEMP_FUEL    = 0x551;
constexpr uint32_t ID_TPMS_PRESSURES       = 0x354;
constexpr uint32_t ID_BODY_CLOSURES        = 0x60D;
constexpr uint32_t ID_LIGHTING_INDICATORS  = 0x625;
constexpr uint32_t ID_TELLTALES_WARNINGS   = 0x650;
constexpr uint32_t ID_OCCUPANTS_ALERTS     = 0x652;
constexpr uint32_t ID_INFOTAINMENT_MFD     = 0x653;
constexpr uint32_t ID_CLIMATE_HVAC         = 0x54C;
constexpr uint32_t ID_VEHICLE_MODE_THEME   = 0x670;

// Encoders and decoders for Engine RPM and Throttle (0x1F9)
inline CanFrame encodeEngine(double rpm, double throttlePercent, bool running) {
    CanFrame f = CanFrame::make(ID_ENGINE_RPM_THROTTLE, 8);
    auto rawRpm = static_cast<uint16_t>(rpm * 4.0);
    f.data[0] = static_cast<uint8_t>((rawRpm >> 8) & 0xFF);
    f.data[1] = static_cast<uint8_t>(rawRpm & 0xFF);
    f.data[2] = static_cast<uint8_t>(throttlePercent);
    f.data[3] = running ? 1 : 0;
    return f;
}

inline void decodeEngine(const CanFrame& f, double& rpm, double& throttlePercent, bool& running) {
    uint16_t rawRpm = (static_cast<uint16_t>(f.data[0]) << 8) | f.data[1];
    rpm = static_cast<double>(rawRpm) / 4.0;
    throttlePercent = static_cast<double>(f.data[2]);
    running = (f.data[3] != 0);
}

// Encoders and decoders for Vehicle Speed and Brake (0x280)
inline CanFrame encodeSpeedBrake(int speedKmh, bool brakePressed, bool parkBrake) {
    CanFrame f = CanFrame::make(ID_VEHICLE_SPEED_BRAKE, 8);
    auto rawSpeed = static_cast<uint16_t>(speedKmh * 100);
    f.data[0] = static_cast<uint8_t>((rawSpeed >> 8) & 0xFF);
    f.data[1] = static_cast<uint8_t>(rawSpeed & 0xFF);
    f.data[2] = brakePressed ? 1 : 0;
    f.data[3] = parkBrake ? 1 : 0;
    return f;
}

inline void decodeSpeedBrake(const CanFrame& f, int& speedKmh, bool& brakePressed, bool& parkBrake) {
    uint16_t rawSpeed = (static_cast<uint16_t>(f.data[0]) << 8) | f.data[1];
    speedKmh = static_cast<int>(rawSpeed / 100);
    brakePressed = (f.data[2] != 0);
    parkBrake = (f.data[3] != 0);
}

// Encoders and decoders for Transmission Gear (0x421)
// Gear enum: 0=P, 1=R, 2=N, 3=D, 4..8=M1..M5, 9..13=D1..D5, 14..18=S1..S5
inline CanFrame encodeGear(const std::string& gear) {
    CanFrame f = CanFrame::make(ID_TRANSMISSION_GEAR, 8);
    uint8_t g = 0;
    if (gear == "P") g = 0;
    else if (gear == "R") g = 1;
    else if (gear == "N") g = 2;
    else if (gear == "D") g = 3;
    else if (gear == "M1" || gear == "1") g = 4;
    else if (gear == "M2" || gear == "2") g = 5;
    else if (gear == "M3" || gear == "3") g = 6;
    else if (gear == "M4" || gear == "4") g = 7;
    else if (gear == "M5" || gear == "5") g = 8;
    else if (gear == "D1") g = 9;
    else if (gear == "D2") g = 10;
    else if (gear == "D3") g = 11;
    else if (gear == "D4") g = 12;
    else if (gear == "D5") g = 13;
    else if (gear == "S1") g = 14;
    else if (gear == "S2") g = 15;
    else if (gear == "S3") g = 16;
    else if (gear == "S4") g = 17;
    else if (gear == "S5") g = 18;
    f.data[0] = g;
    return f;
}

inline std::string decodeGear(const CanFrame& f) {
    switch (f.data[0]) {
    case 0: return "P";
    case 1: return "R";
    case 2: return "N";
    case 3: return "D";
    case 4: return "M1";
    case 5: return "M2";
    case 6: return "M3";
    case 7: return "M4";
    case 8: return "M5";
    case 9: return "D1";
    case 10: return "D2";
    case 11: return "D3";
    case 12: return "D4";
    case 13: return "D5";
    case 14: return "S1";
    case 15: return "S2";
    case 16: return "S3";
    case 17: return "S4";
    case 18: return "S5";
    default: return "P";
    }
}

// Encoders and decoders for Coolant, Fuel, Ambient Temp and Odometer (0x551)
inline CanFrame encodeCoolantFuel(int coolantTemp, int fuelPercent, int ambientTemp, int odoKm, int dteKm) {
    CanFrame f = CanFrame::make(ID_COOLANT_TEMP_FUEL, 8);
    f.data[0] = static_cast<uint8_t>(coolantTemp + 40);
    f.data[1] = static_cast<uint8_t>(fuelPercent);
    f.data[2] = static_cast<uint8_t>(ambientTemp + 40);
    f.data[3] = static_cast<uint8_t>((dteKm / 10) & 0xFF);
    auto rawOdo = static_cast<uint16_t>(odoKm / 10);
    f.data[4] = static_cast<uint8_t>((rawOdo >> 8) & 0xFF);
    f.data[5] = static_cast<uint8_t>(rawOdo & 0xFF);
    return f;
}

inline void decodeCoolantFuel(const CanFrame& f, int& coolantTemp, int& fuelPercent, int& ambientTemp, int& odoKm, int& dteKm) {
    coolantTemp = static_cast<int>(f.data[0]) - 40;
    fuelPercent = static_cast<int>(f.data[1]);
    ambientTemp = static_cast<int>(f.data[2]) - 40;
    dteKm = static_cast<int>(f.data[3]) * 10;
    uint16_t rawOdo = (static_cast<uint16_t>(f.data[4]) << 8) | f.data[5];
    odoKm = static_cast<int>(rawOdo) * 10;
}

// Encoders and decoders for TPMS Pressures (0x354)
inline CanFrame encodeTpms(double fl, double fr, double rl, double rr) {
    CanFrame f = CanFrame::make(ID_TPMS_PRESSURES, 8);
    f.data[0] = static_cast<uint8_t>(fl * 2.0);
    f.data[1] = static_cast<uint8_t>(fr * 2.0);
    f.data[2] = static_cast<uint8_t>(rl * 2.0);
    f.data[3] = static_cast<uint8_t>(rr * 2.0);
    return f;
}

inline void decodeTpms(const CanFrame& f, double& fl, double& fr, double& rl, double& rr) {
    fl = static_cast<double>(f.data[0]) / 2.0;
    fr = static_cast<double>(f.data[1]) / 2.0;
    rl = static_cast<double>(f.data[2]) / 2.0;
    rr = static_cast<double>(f.data[3]) / 2.0;
}

// Encoders and decoders for Body Closures, Locks and Seatbelts (0x60D)
inline CanFrame encodeBodyClosures(bool flOpen, bool frOpen, bool rlOpen, bool rrOpen,
                                  bool hoodOpen, bool trunkOpen, bool centralLock,
                                  bool driverSeatbelt, bool passSeatbelt) {
    CanFrame f = CanFrame::make(ID_BODY_CLOSURES, 8);
    uint8_t doors = 0;
    if (flOpen) doors |= (1 << 0);
    if (frOpen) doors |= (1 << 1);
    if (rlOpen) doors |= (1 << 2);
    if (rrOpen) doors |= (1 << 3);
    f.data[0] = doors;

    uint8_t exterior = 0;
    if (hoodOpen) exterior |= (1 << 0);
    if (trunkOpen) exterior |= (1 << 1);
    if (centralLock) exterior |= (1 << 2);
    f.data[1] = exterior;

    uint8_t restraints = 0;
    if (driverSeatbelt) restraints |= (1 << 0);
    if (passSeatbelt) restraints |= (1 << 1);
    f.data[2] = restraints;
    return f;
}

inline void decodeBodyClosures(const CanFrame& f, bool& flOpen, bool& frOpen, bool& rlOpen, bool& rrOpen,
                               bool& hoodOpen, bool& trunkOpen, bool& centralLock,
                               bool& driverSeatbelt, bool& passSeatbelt) {
    flOpen = (f.data[0] & (1 << 0)) != 0;
    frOpen = (f.data[0] & (1 << 1)) != 0;
    rlOpen = (f.data[0] & (1 << 2)) != 0;
    rrOpen = (f.data[0] & (1 << 3)) != 0;

    hoodOpen = (f.data[1] & (1 << 0)) != 0;
    trunkOpen = (f.data[1] & (1 << 1)) != 0;
    centralLock = (f.data[1] & (1 << 2)) != 0;

    driverSeatbelt = (f.data[2] & (1 << 0)) != 0;
    passSeatbelt = (f.data[2] & (1 << 1)) != 0;
}

// Encoders and decoders for Lighting and Indicators (0x625)
inline CanFrame encodeLighting(bool leftIndicator, bool rightIndicator,
                               bool lowBeam, bool highBeam, bool posLamp, int lightMode) {
    CanFrame f = CanFrame::make(ID_LIGHTING_INDICATORS, 8);
    uint8_t ind = 0;
    if (leftIndicator) ind |= (1 << 0);
    if (rightIndicator) ind |= (1 << 1);
    f.data[0] = ind;

    uint8_t lamps = 0;
    if (lowBeam) lamps |= (1 << 0);
    if (highBeam) lamps |= (1 << 1);
    if (posLamp) lamps |= (1 << 2);
    f.data[1] = lamps;

    f.data[2] = static_cast<uint8_t>(lightMode);
    return f;
}

inline void decodeLighting(const CanFrame& f, bool& leftIndicator, bool& rightIndicator,
                           bool& lowBeam, bool& highBeam, bool& posLamp, int& lightMode) {
    leftIndicator = (f.data[0] & (1 << 0)) != 0;
    rightIndicator = (f.data[0] & (1 << 1)) != 0;
    lowBeam = (f.data[1] & (1 << 0)) != 0;
    highBeam = (f.data[1] & (1 << 1)) != 0;
    posLamp = (f.data[1] & (1 << 2)) != 0;
    lightMode = static_cast<int>(f.data[2]);
}

// Encoders and decoders for Telltales and Warnings (0x650)
inline CanFrame encodeTelltales(bool abs, bool airbag, bool battery, bool oil,
                                bool checkEngine, bool esc, bool escOff,
                                bool steering, bool masterWarning,
                                bool lightWarning = false, bool tpmsActive = false) {
    CanFrame f = CanFrame::make(ID_TELLTALES_WARNINGS, 8);
    uint8_t t1 = 0;
    if (abs) t1 |= (1 << 0);
    if (airbag) t1 |= (1 << 1);
    if (battery) t1 |= (1 << 2);
    if (oil) t1 |= (1 << 3);
    if (checkEngine) t1 |= (1 << 4);
    if (esc) t1 |= (1 << 5);
    if (escOff) t1 |= (1 << 6);
    f.data[0] = t1;

    uint8_t t2 = 0;
    if (steering) t2 |= (1 << 0);
    if (masterWarning) t2 |= (1 << 1);
    if (lightWarning) t2 |= (1 << 2);
    if (tpmsActive) t2 |= (1 << 3);
    f.data[1] = t2;
    return f;
}

inline void decodeTelltales(const CanFrame& f, bool& abs, bool& airbag, bool& battery, bool& oil,
                            bool& checkEngine, bool& esc, bool& escOff,
                            bool& steering, bool& masterWarning,
                            bool& lightWarning, bool& tpmsActive) {
    abs = (f.data[0] & (1 << 0)) != 0;
    airbag = (f.data[0] & (1 << 1)) != 0;
    battery = (f.data[0] & (1 << 2)) != 0;
    oil = (f.data[0] & (1 << 3)) != 0;
    checkEngine = (f.data[0] & (1 << 4)) != 0;
    esc = (f.data[0] & (1 << 5)) != 0;
    escOff = (f.data[0] & (1 << 6)) != 0;

    steering = (f.data[1] & (1 << 0)) != 0;
    masterWarning = (f.data[1] & (1 << 1)) != 0;
    lightWarning = (f.data[1] & (1 << 2)) != 0;
    tpmsActive = (f.data[1] & (1 << 3)) != 0;
}

// Encoders and decoders for Occupants, Prompts and Alerts (0x652)
inline CanFrame encodeOccupantsAlerts(bool cruiseEnabled, bool rearLeftBuckled, bool rearCenterBuckled, bool rearRightBuckled,
                                      bool rearAlarmActive, int smartKeyPrompt, int startPedalPrompt,
                                      bool pressStartAgainAlert, bool driverAttentionActive, bool reduceSpeedAlert,
                                      bool sunroofOpen, bool sunroofAlertActive) {
    CanFrame f = CanFrame::make(ID_OCCUPANTS_ALERTS, 8);
    uint8_t b0 = 0;
    if (cruiseEnabled) b0 |= (1 << 0);
    if (rearLeftBuckled) b0 |= (1 << 1);
    if (rearCenterBuckled) b0 |= (1 << 2);
    if (rearRightBuckled) b0 |= (1 << 3);
    if (rearAlarmActive) b0 |= (1 << 4);
    f.data[0] = b0;

    f.data[1] = static_cast<uint8_t>((smartKeyPrompt & 0x0F) | ((startPedalPrompt & 0x0F) << 4));

    uint8_t b2 = 0;
    if (pressStartAgainAlert) b2 |= (1 << 0);
    if (driverAttentionActive) b2 |= (1 << 1);
    if (reduceSpeedAlert) b2 |= (1 << 2);
    if (sunroofOpen) b2 |= (1 << 3);
    if (sunroofAlertActive) b2 |= (1 << 4);
    f.data[2] = b2;
    return f;
}

inline void decodeOccupantsAlerts(const CanFrame& f, bool& cruiseEnabled, bool& rearLeftBuckled, bool& rearCenterBuckled, bool& rearRightBuckled,
                                  bool& rearAlarmActive, int& smartKeyPrompt, int& startPedalPrompt,
                                  bool& pressStartAgainAlert, bool& driverAttentionActive, bool& reduceSpeedAlert,
                                  bool& sunroofOpen, bool& sunroofAlertActive) {
    cruiseEnabled = (f.data[0] & (1 << 0)) != 0;
    rearLeftBuckled = (f.data[0] & (1 << 1)) != 0;
    rearCenterBuckled = (f.data[0] & (1 << 2)) != 0;
    rearRightBuckled = (f.data[0] & (1 << 3)) != 0;
    rearAlarmActive = (f.data[0] & (1 << 4)) != 0;

    smartKeyPrompt = static_cast<int>(f.data[1] & 0x0F);
    startPedalPrompt = static_cast<int>((f.data[1] >> 4) & 0x0F);

    pressStartAgainAlert = (f.data[2] & (1 << 0)) != 0;
    driverAttentionActive = (f.data[2] & (1 << 1)) != 0;
    reduceSpeedAlert = (f.data[2] & (1 << 2)) != 0;
    sunroofOpen = (f.data[2] & (1 << 3)) != 0;
    sunroofAlertActive = (f.data[2] & (1 << 4)) != 0;
}

// Encoders and decoders for Infotainment and MFD Navigation (0x653)
inline CanFrame encodeInfotainmentMfd(bool isMediaPlaying, const std::string& mediaSource, bool showMediaPopup,
                                      int menuTab, int tripPage, uint8_t commandAction = 0) {
    CanFrame f = CanFrame::make(ID_INFOTAINMENT_MFD, 8);
    uint8_t b0 = 0;
    if (isMediaPlaying) b0 |= (1 << 0);
    if (showMediaPopup) b0 |= (1 << 1);
    f.data[0] = b0;

    uint8_t srcCode = 0;
    if (mediaSource == "Bluetooth") srcCode = 0;
    else if (mediaSource == "USB") srcCode = 1;
    else if (mediaSource == "Apple CarPlay") srcCode = 2;
    else if (mediaSource == "Android Auto") srcCode = 3;
    else if (mediaSource == "FM Radio") srcCode = 4;
    else if (mediaSource == "Radio") srcCode = 5;
    f.data[1] = srcCode;

    f.data[2] = static_cast<uint8_t>(menuTab & 0xFF);
    f.data[3] = static_cast<uint8_t>(tripPage & 0xFF);
    f.data[4] = commandAction;
    return f;
}

inline void decodeInfotainmentMfd(const CanFrame& f, bool& isMediaPlaying, std::string& mediaSource, bool& showMediaPopup,
                                  int& menuTab, int& tripPage, uint8_t& commandAction) {
    isMediaPlaying = (f.data[0] & (1 << 0)) != 0;
    showMediaPopup = (f.data[0] & (1 << 1)) != 0;

    switch (f.data[1]) {
    case 0: mediaSource = "Bluetooth"; break;
    case 1: mediaSource = "USB"; break;
    case 2: mediaSource = "Apple CarPlay"; break;
    case 3: mediaSource = "Android Auto"; break;
    case 4: mediaSource = "FM Radio"; break;
    case 5: mediaSource = "Radio"; break;
    default: mediaSource = "Bluetooth"; break;
    }

    menuTab = static_cast<int>(f.data[2]);
    tripPage = static_cast<int>(f.data[3]);
    commandAction = f.data[4];
}

// Encoders and decoders for Climate / HVAC (0x54C)
inline CanFrame encodeClimate(double driverTemp, double passTemp, int fanSpeed, int airflowMode,
                              bool ac, bool autoMode, bool dual, bool recirc,
                              bool frontDefrost, bool rearDefrost,
                              int driverSeatHeat, int passSeatHeat, bool eco = false) {
    CanFrame f = CanFrame::make(ID_CLIMATE_HVAC, 8);
    f.data[0] = static_cast<uint8_t>(driverTemp * 2.0);
    f.data[1] = static_cast<uint8_t>(passTemp * 2.0);
    f.data[2] = static_cast<uint8_t>(fanSpeed);
    f.data[3] = static_cast<uint8_t>(airflowMode);

    uint8_t flags = 0;
    if (ac) flags |= (1 << 0);
    if (autoMode) flags |= (1 << 1);
    if (dual) flags |= (1 << 2);
    if (recirc) flags |= (1 << 3);
    if (frontDefrost) flags |= (1 << 4);
    if (rearDefrost) flags |= (1 << 5);
    if (eco) flags |= (1 << 6);
    f.data[4] = flags;

    f.data[5] = static_cast<uint8_t>(driverSeatHeat);
    f.data[6] = static_cast<uint8_t>(passSeatHeat);
    return f;
}

inline void decodeClimate(const CanFrame& f, double& driverTemp, double& passTemp, int& fanSpeed, int& airflowMode,
                          bool& ac, bool& autoMode, bool& dual, bool& recirc,
                          bool& frontDefrost, bool& rearDefrost,
                          int& driverSeatHeat, int& passSeatHeat, bool& eco) {
    driverTemp = static_cast<double>(f.data[0]) / 2.0;
    passTemp = static_cast<double>(f.data[1]) / 2.0;
    fanSpeed = static_cast<int>(f.data[2]);
    airflowMode = static_cast<int>(f.data[3]);

    ac = (f.data[4] & (1 << 0)) != 0;
    autoMode = (f.data[4] & (1 << 1)) != 0;
    dual = (f.data[4] & (1 << 2)) != 0;
    recirc = (f.data[4] & (1 << 3)) != 0;
    frontDefrost = (f.data[4] & (1 << 4)) != 0;
    rearDefrost = (f.data[4] & (1 << 5)) != 0;
    eco = (f.data[4] & (1 << 6)) != 0;

    driverSeatHeat = static_cast<int>(f.data[5]);
    passSeatHeat = static_cast<int>(f.data[6]);
}

// Encoders and decoders for Vehicle Mode and Theme (0x670)
inline CanFrame encodeModeTheme(int clusterState, const std::string& themeColor) {
    CanFrame f = CanFrame::make(ID_VEHICLE_MODE_THEME, 8);
    f.data[0] = static_cast<uint8_t>(clusterState);
    uint8_t themeCode = 0;
    if (themeColor == "blue") themeCode = 0;
    else if (themeColor == "green") themeCode = 1;
    else if (themeColor == "red") themeCode = 2;
    f.data[1] = themeCode;
    return f;
}

inline void decodeModeTheme(const CanFrame& f, int& clusterState, std::string& themeColor) {
    clusterState = static_cast<int>(f.data[0]);
    switch (f.data[1]) {
    case 0: themeColor = "blue"; break;
    case 1: themeColor = "green"; break;
    case 2: themeColor = "red"; break;
    default: themeColor = "blue"; break;
    }
}

} // namespace NissanCan
