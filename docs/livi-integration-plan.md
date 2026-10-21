# LIVI Integration Plan — Infiniti G35 Triple Display System

Target hardware is Radxa ROCK 5B Plus with RK3588 ARM and Mali-G610 and Linux Wayland and SocketCAN.
Target displays are 1920x1080 head unit plus 1920x720 cluster plus 1920x480 climate ribbon.
LIVI version inspected is 9.5.0 with GPL-3.0-or-later license and ARM64 AppImage support.
Date of this plan is 2026-10-18.

## 1. What LIVI Is

LIVI is an open source Apple CarPlay and Android Auto head unit from f-io.
Main process is Electron with TypeScript and UI is React with Main plus Cluster plus Aux apps.
Native layer is Rust with livi-helperd for phone protocols and livi-gst-video for zero copy hardware decoded video and livi-compositor for Smithay Wayland output.
Phone support covers wired and wireless CarPlay including Ultra cluster and climate plus wired and wireless Android Auto with multitouch and D-Pad and audio and microphone and GPS forwarding.

## 2. Findings

F1. LIVI covers phone projection and navigation video and media and calls and voice and Ultra cluster and climate while it has no Nissan FM CAN decoding and no safety critical gauge logic.
F2. VehicleState in core remains the vehicle truth and LIVI accepts vehicle data only as telemetry push over HTTP POST to port 4000 slash telemetry or over Socket.IO event telemetry push with camelCase JSON defined in telemetry dot rs.
F3. Display models align because LIVI supports Main Screen plus Dash Screen plus Aux Screen as separate placeable windows while this project needs head unit plus cluster plus climate ribbon.
F4. Climate sync is two way because phone edits land in telemetry and external pushes reach the phone and last state persists in state slash climate dot json.
F5. Wireless needs WiFi access point plus Bluetooth plus USB and wired CarPlay needs MFi authentication from an I2C coprocessor or from a LIVI Link dongle and macOS dev hosts require the Link dongle because they provide no I2C bus.
F6. Compositor choice is exclusive because LIVI ships Smithay compositor on Linux and this project plans system Weston and only one compositor can own triple output placement per boot.
F7. ARM64 packaging exists because LIVI builds linux arm64 AppImage with bundled GStreamer 1.28.8 which matches ROCK 5B Plus architecture.

## 3. Display Mapping for ROCK 5B Plus

ROCK 5B Plus provides two HDMI outputs plus one USB-C DisplayPort alt mode plus MIPI DSI which allows three independent outputs without a splitter.
Exact Wayland output names must be confirmed with wayland-info on Radxa Debian and then pinned in weston dot ini.

| Output | Resolution | Content | Owner |
|--------|------------|---------|-------|
| HDMI0 | 1920x1080 | Phone projection and LIVI Main | LIVI |
| HDMI1 | 1920x720 | Speed plus RPM plus telltales | Qt ApexClusterApp |
| USB-C DP | 1920x480 | Dual zone climate plus clock | Qt InfinitiClimateApp |

Qt cluster stays authoritative for safety and Qt climate stays native because 1920x480 ultra wide has no LIVI layout.
LIVI Dash cluster stream is enabled later only as a background behind Qt gauges after latency is measured.

## 4. Options

O1. Sidecar keeps ApexClusterApp plus SmartUiDashboard plus InfinitiClimateApp unchanged and runs unmodified LIVI AppImage alongside as phone projection on the head unit output.
O2. Partial replacement keeps Qt cluster and Qt climate ribbon and assigns LIVI Main to own the full 1920x1080 head unit output.
O3. Full takeover uses LIVI Main plus Dash plus Aux for all three outputs and keeps Qt apps only as fallback when no phone is connected.

## 5. Decisions

D1. Start with O1 for the first milestone and then move to O2 because that path preserves safety UI while proving projection on target hardware.
D2. Keep VehicleState as the single CAN owner and add a bridge publisher and allow no direct CAN access from LIVI.
D3. Run under system Weston first with livi-compositor disabled and enable LIVI compositor only if Weston cannot place three kiosk windows reliably.
D4. Treat unmodified sidecar AppImage as the lowest GPL risk path and resolve copyleft duties before any fork or static link or distribution.

## 6. Telemetry Bridge Design

A new QObject named LiviTelemetryBridge in core subscribes to VehicleState signals and pushes JSON on change at 10 Hz plus 1 Hz heartbeat.
Transport is Qt Network POST to 127 dot 0 dot 0 dot 1 port 4000 slash telemetry with Socket.IO subscribe or state file poll as fallback for phone to car climate.
Reverse path applies incoming climate updates to VehicleState setters and then to CAN broadcast so physical HVAC follows the phone.

| VehicleState | LIVI telemetry | Notes |
|--------------|----------------|-------|
| speed | speedKph | Direct value |
| rpm | rpm | Direct value |
| gear QString | gear | String for automatic and number for manual |
| coolantTemp | coolantC | Direct value |
| ambientTemp | ambientC | Shared with phone |
| fuelLevel | fuelPct | Percent scale |
| dteKm | rangeKm | Remaining distance |
| odoKm | odometerKm | Odometer |
| throttlePercent | throttlePct | Percent scale |
| TPMS psi values | tirePressure front and rear kPa | Multiply psi by 6 dot 89476 |
| leftIndicator and rightIndicator | turnSignal | Left or right or none |
| lowBeam | lights | Low beam state |
| highBeam | highBeam | High beam state |
| parkBrakeActive | parkingBrake | Parking brake state |
| driverTemp | climate driverTargetC | Celsius with 0 dot 5 steps |
| passengerTemp | climate passengerTargetC | Celsius with 0 dot 5 steps |
| fanSpeed | climate fanLevel | Range 0 to 7 |
| autoActive | climate auto | Auto mode flag |
| acActive | climate ac | Air conditioning flag |
| recircActive | climate recirculation | Recirculation flag |
| dualMode inverted | climate zonesSynced | Sync flag |
| frontDefrostActive | climate frontWindowHeating | Binary flag |
| rearDefrostActive | climate rearWindowHeating | Binary flag |
| airflowMode | climate vent upper plus middle plus lower | Map mode to vents |
| driverSeatHeat and passengerSeatHeat | climate seat heating levels | Range 0 to 2 |

## 7. Actions

A1. Complete license check and confirm sidecar AppImage use satisfies GPL duties and document the result.
A2. Complete ARM64 spike by building LIVI for linux arm64 and running the AppImage with bundled GStreamer and verifying GPU decode and three window placement under Weston.
A3. Complete one way bridge by implementing LiviTelemetryBridge with HTTP POST for powertrain plus lights plus TPMS and verifying with livi-core telemetry demo and curl against port 4000.
A4. Complete display routing by pinning LIVI Main to 1920x1080 and Qt cluster to 1920x720 and Qt climate to 1920x480 and adding justfile targets for run-livi and run-all-with-livi.
A5. Complete two way climate by subscribing to telemetry updates and applying phone edits to VehicleState and testing AUTO plus AC plus fan plus sync plus seat heat in both directions.
A6. Complete connectivity by selecting MFi route with Link dongle or I2C chip and testing wired CarPlay plus wireless CarPlay plus wired Android Auto plus wireless Android Auto plus audio plus microphone plus GPS forwarding.
A7. Complete hardening by setting startup order with CAN plus bridge plus Weston plus Qt safety UI first plus LIVI second and adding watchdog and persisting config plus climate state and measuring CPU plus GPU plus memory plus frame latency on RK3588.

## 8. Risks

R1. GPL-3.0 copyleft applies on distribution if LIVI is modified or statically linked.
R2. Compositor conflict exists between Weston and livi-compositor for triple output placement.
R3. WiFi access point ownership fights NetworkManager and needs dedicated interface mode.
R4. MFi chip procurement and I2C plus GPIO wiring delay wired CarPlay.
R5. Electron plus Qt plus GStreamer contention on RK3588 causes dropped frames or audio delay.
R6. Cluster safety is affected if LIVI Dash overlays Qt gauges so Qt cluster must stay authoritative until Ultra timing is measured.
R7. Ultra wide 1920x480 has no LIVI layout so climate must stay Qt with sync rather than native LIVI Aux.

## 9. Outcome

Sidecar first keeps risk low and preserves safety UI while proving phone projection on ROCK 5B Plus.
Bridge work is small and isolated and display mapping is direct and reversible.
Keep Qt CAN plus cluster plus climate and add LIVI only as the phone projection head unit behind a small telemetry bridge.
