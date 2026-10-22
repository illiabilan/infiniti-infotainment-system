#include "NissanCanDefinitions.h"
#include "CanBackendFactory.h"
#include "CoreClient.h"
#include "VehicleChangeWatcher.h"
#include "VehicleState.h"
#include "MockCanBackend.h"
#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QThread>
#include <QVariant>
#include <cassert>
#include <iostream>
#include <cmath>

static void testCodecs() {
    std::cout << "[TEST] Running Nissan CAN Codec verification..." << std::endl;

    // 1. Engine
    {
        CanFrame f = NissanCan::encodeEngine(3250.5, 45.0, true);
        double rpm = 0.0;
        double throttle = 0.0;
        bool running = false;
        NissanCan::decodeEngine(f, rpm, throttle, running);
        assert(std::abs(rpm - 3250.5) < 0.5);
        assert(std::abs(throttle - 45.0) < 1.0);
        assert(running == true);
    }

    // 2. Speed and Brake
    {
        CanFrame f = NissanCan::encodeSpeedBrake(120, true, false);
        int spd = 0;
        bool brake = false;
        bool park = false;
        NissanCan::decodeSpeedBrake(f, spd, brake, park);
        assert(spd == 120);
        assert(brake == true);
        assert(park == false);
    }

    // 3. Gear
    {
        CanFrame f1 = NissanCan::encodeGear("D");
        assert(NissanCan::decodeGear(f1) == "D");
        CanFrame f2 = NissanCan::encodeGear("M3");
        assert(NissanCan::decodeGear(f2) == "M3");
        CanFrame f3 = NissanCan::encodeGear("D4");
        assert(NissanCan::decodeGear(f3) == "D4");
        CanFrame f4 = NissanCan::encodeGear("S2");
        assert(NissanCan::decodeGear(f4) == "S2");
    }

    // 4. Coolant, Fuel, Ambient, Odometer
    {
        CanFrame f = NissanCan::encodeCoolantFuel(92, 75, 18, 142850, 480);
        int coolant = 0, fuel = 0, ambient = 0, odo = 0, dte = 0;
        NissanCan::decodeCoolantFuel(f, coolant, fuel, ambient, odo, dte);
        assert(coolant == 92);
        assert(fuel == 75);
        assert(ambient == 18);
        assert(odo == 142850);
        assert(dte == 480);
    }

    // 5. TPMS
    {
        CanFrame f = NissanCan::encodeTpms(34.5, 35.0, 33.5, 34.0);
        double fl = 0, fr = 0, rl = 0, rr = 0;
        NissanCan::decodeTpms(f, fl, fr, rl, rr);
        assert(std::abs(fl - 34.5) < 0.2);
        assert(std::abs(fr - 35.0) < 0.2);
        assert(std::abs(rl - 33.5) < 0.2);
        assert(std::abs(rr - 34.0) < 0.2);
    }

    // 6. Body Closures
    {
        CanFrame f = NissanCan::encodeBodyClosures(true, false, false, true, true, false, true, true, false);
        bool fl = false, fr = false, rl = false, rr = false;
        bool hood = false, trunk = false, centralLock = false;
        bool driverBelt = false, passBelt = false;
        NissanCan::decodeBodyClosures(f, fl, fr, rl, rr, hood, trunk, centralLock, driverBelt, passBelt);
        assert(fl == true);
        assert(fr == false);
        assert(rl == false);
        assert(rr == true);
        assert(hood == true);
        assert(trunk == false);
        assert(centralLock == true);
        assert(driverBelt == true);
        assert(passBelt == false);
    }

    // 7. Climate
    {
        CanFrame f = NissanCan::encodeClimate(22.5, 23.0, 4, 1, true, false, true, true, false, true, 2, 0, true);
        double dTemp = 0.0, pTemp = 0.0;
        int fan = 0, airflow = 0;
        bool ac = false, autoMode = false, dual = false, recirc = false;
        bool fDef = false, rDef = false;
        int dHeat = 0, pHeat = 0;
        bool eco = false;
        NissanCan::decodeClimate(f, dTemp, pTemp, fan, airflow, ac, autoMode, dual, recirc, fDef, rDef, dHeat, pHeat, eco);
        assert(std::abs(dTemp - 22.5) < 0.2);
        assert(std::abs(pTemp - 23.0) < 0.2);
        assert(fan == 4);
        assert(airflow == 1);
        assert(ac == true);
        assert(autoMode == false);
        assert(dual == true);
        assert(recirc == true);
        assert(fDef == false);
        assert(rDef == true);
        assert(dHeat == 2);
        assert(pHeat == 0);
        assert(eco == true);
    }

    // 8. Mode & Theme
    {
        CanFrame f = NissanCan::encodeModeTheme(3, "green");
        int state = 0;
        std::string theme;
        NissanCan::decodeModeTheme(f, state, theme);
        assert(state == 3);
        assert(theme == "green");
    }

    // 9. Telltales (including lightWarning & tpmsActive)
    {
        CanFrame f = NissanCan::encodeTelltales(true, false, true, false, true, false, true, false, true, true, true);
        bool abs = false, vdc = false, vdcOff = false, engine = false, oil = false;
        bool battery = false, airbag = false, steering = false, master = false, lightWarn = false, tpms = false;
        NissanCan::decodeTelltales(f, abs, vdc, vdcOff, engine, oil, battery, airbag, steering, master, lightWarn, tpms);
        assert(abs == true);
        assert(vdc == false);
        assert(vdcOff == true);
        assert(engine == false);
        assert(oil == true);
        assert(battery == false);
        assert(airbag == true);
        assert(steering == false);
        assert(master == true);
        assert(lightWarn == true);
        assert(tpms == true);
    }

    // 10. Occupants & Alerts (0x652)
    {
        CanFrame f = NissanCan::encodeOccupantsAlerts(true, false, true, false, true, 3, 2, true, true, true, true, true);
        bool cruiseEn = false, rl = true, rc = false, rr = true, rearAlarm = false;
        int skPrompt = 0, spPrompt = 0;
        bool pressStart = false, driverAtt = false, reduceSpd = false, srOpen = false, srAlert = false;
        NissanCan::decodeOccupantsAlerts(f, cruiseEn, rl, rc, rr, rearAlarm, skPrompt, spPrompt, pressStart, driverAtt, reduceSpd, srOpen, srAlert);
        assert(cruiseEn == true);
        assert(rl == false);
        assert(rc == true);
        assert(rr == false);
        assert(rearAlarm == true);
        assert(skPrompt == 3);
        assert(spPrompt == 2);
        assert(pressStart == true);
        assert(driverAtt == true);
        assert(reduceSpd == true);
        assert(srOpen == true);
        assert(srAlert == true);
    }

    // 11. Infotainment & MFD (0x653)
    {
        CanFrame f = NissanCan::encodeInfotainmentMfd(true, "FM Radio", true, 2, 1, 3);
        bool playing = false, popup = false;
        std::string src;
        int tab = 0, page = 0;
        uint8_t cmd = 0;
        NissanCan::decodeInfotainmentMfd(f, playing, src, popup, tab, page, cmd);
        assert(playing == true);
        assert(src == "FM Radio");
        assert(popup == true);
        assert(tab == 2);
        assert(page == 1);
        assert(cmd == 3);
    }

    std::cout << "[TEST] All Nissan CAN Codec assertions PASSED." << std::endl;
}

static void testVehicleState() {
    std::cout << "[TEST] Running VehicleState ingest verification..." << std::endl;
    VehicleState state;

    // Simulate incoming Speed & Brake CAN frame
    CanFrame fSpeed = NissanCan::encodeSpeedBrake(88, false, false);
    state.processFrame(fSpeed);
    assert(state.speed() == 88);
    assert(state.brakePressed() == false);

    // Simulate incoming RPM CAN frame (normalized 0.0-8.0 x1000 RPM)
    CanFrame fRpm = NissanCan::encodeEngine(4500.0, 60.0, true);
    state.processFrame(fRpm);
    assert(std::abs(state.rpm() - 4.5) < 0.05);
    assert(state.engineRunning() == true);

    // Simulate incoming Climate CAN frame
    CanFrame fClimate = NissanCan::encodeClimate(21.0, 21.0, 2, 0, true, true, false, false, false, false, 0, 0, true);
    state.processFrame(fClimate);
    assert(std::abs(state.driverTemp() - 21.0) < 0.2);
    assert(state.property("outsideTemp").toInt() == state.ambientTemp());
    assert(state.fanSpeed() == 2);
    assert(state.acActive() == true);
    assert(state.dualMode() == false);
    assert(state.ecoMode() == true);

    // Simulate incoming Occupants & Alerts CAN frame (0x652)
    CanFrame fAlerts = NissanCan::encodeOccupantsAlerts(true, false, true, true, true, 2, 1, true, true, false, true, true);
    state.processFrame(fAlerts);
    assert(state.cruiseEnabled() == true);
    assert(state.rearLeftBuckled() == false);
    assert(state.rearAlarmActive() == true);
    assert(state.smartKeyPrompt() == 2);
    assert(state.startPedalPrompt() == 1);
    assert(state.pressStartAgainAlert() == true);
    assert(state.driverAttentionActive() == true);
    assert(state.sunroofOpen() == true);
    assert(state.sunroofAlertActive() == true);

    // Simulate incoming Infotainment & MFD CAN frame (0x653)
    bool nextFired = false;
    QObject::connect(&state, &VehicleState::mediaNextRequested, [&nextFired]() { nextFired = true; });
    CanFrame fInfo = NissanCan::encodeInfotainmentMfd(true, "FM Radio", true, 2, 1, 1);
    state.processFrame(fInfo);
    assert(state.isMediaPlaying() == true);
    assert(state.mediaSource() == QStringLiteral("FM Radio"));
    assert(state.showMediaPopup() == true);
    assert(state.menuTab() == 2);
    assert(state.tripPage() == 1);
    assert(nextFired == true);

    std::cout << "[TEST] VehicleState assertions PASSED." << std::endl;
}

static void testMockCanLoopback() {
    std::cout << "[TEST] Running MockCanBackend loopback transmission verification..." << std::endl;
    MockCanBackend sender;
    MockCanBackend receiver;

    bool senderOpened = sender.open();
    assert(senderOpened == true);

    bool receiverOpened = receiver.open();
    assert(receiverOpened == true);

    VehicleState targetState;
    targetState.attachCanInterface(&receiver);

    // Send RPM frame via sender
    CanFrame f = NissanCan::encodeEngine(5800.0, 80.0, true);
    bool sent = sender.sendFrame(f);
    assert(sent == true);

    // Process Qt event loop to receive datagram
    for (int i = 0; i < 50; ++i) {
        QCoreApplication::processEvents();
        if (std::abs(targetState.rpm() - 5.8) < 0.05) break;
        QThread::msleep(5);
    }

    assert(std::abs(targetState.rpm() - 5.8) < 0.05);
    assert(targetState.engineRunning() == true);

    sender.close();
    receiver.close();
    std::cout << "[TEST] MockCanBackend loopback transmission PASSED." << std::endl;
}

// AC3 trace: prints the change log of the watched keys until one drive cycle ends.
static const QStringList TRACE_KEYS{
    QStringLiteral("speed"), QStringLiteral("rpm"),
    QStringLiteral("driverTemp"), QStringLiteral("passengerTemp"), QStringLiteral("fanSpeed"),
    QStringLiteral("airflowMode"), QStringLiteral("acActive"), QStringLiteral("autoActive"),
    QStringLiteral("dualMode"), QStringLiteral("recircActive"), QStringLiteral("ecoMode"),
    QStringLiteral("frontDefrostActive"), QStringLiteral("rearDefrostActive"),
    QStringLiteral("driverSeatHeat"), QStringLiteral("passengerSeatHeat")
};

static QString traceValue(const VehicleModel &model, const QString &key) {
    const QVariant value = model.property(key.toLatin1().constData());
    switch (value.typeId()) {
    case QMetaType::Double: return QString::number(value.toDouble(), 'f', 2);
    case QMetaType::Bool: return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    default: return QString::number(value.toInt());
    }
}

// The emulator drive demo rounds its speed smoothing to int and settles at 5 km/h, never at 0.
static constexpr int TRACE_CRUISE_SPEED = 50;
static constexpr int TRACE_REST_SPEED = 5;

static int runTrace(QCoreApplication &app, VehicleModel &model) {
    VehicleChangeWatcher watcher(&model);
    bool sawMotion = false;
    bool finished = false;

    QObject::connect(&watcher, &VehicleChangeWatcher::valuesChanged, [&](const QJsonObject &values) {
        if (finished) return;
        for (const QString &key : TRACE_KEYS) {
            if (values.contains(key)) {
                std::cout << key.toStdString() << "=" << traceValue(model, key).toStdString() << std::endl;
            }
        }
        if (!values.contains(QStringLiteral("speed"))) return;

        if (model.speed() >= TRACE_CRUISE_SPEED) {
            sawMotion = true;
        } else if (sawMotion && model.speed() <= TRACE_REST_SPEED) {
            std::cout << "final";
            for (const QString &key : TRACE_KEYS) {
                std::cout << " " << key.toStdString() << "=" << traceValue(model, key).toStdString();
            }
            std::cout << std::endl;
            finished = true;
            app.exit(0);
        }
    });

    QTimer::singleShot(300000, [&]() {
        std::cerr << "TRACE_TIMEOUT" << std::endl;
        app.exit(1);
    });
    return app.exec();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc > 1 && std::string(argv[1]) == "listen") {
        MockCanBackend receiver;
        receiver.open();
        int frameCount = 0;
        QObject::connect(&receiver, &CanInterface::frameReceived, [&frameCount, &app](const CanFrame &f) {
            std::cout << "RECV_FRAME: ID=0x" << std::hex << f.canId << std::dec << " DLC=" << int(f.dlc) << std::endl;
            frameCount++;
            if (frameCount >= 10) {
                std::cout << "RECEIVED_HEARTBEAT_FRAMES_OK: " << frameCount << std::endl;
                app.quit();
            }
        });
        QTimer::singleShot(3000, [&]() {
            std::cout << "LISTEN_TIMEOUT, total frames: " << frameCount << std::endl;
            app.exit(frameCount > 0 ? 0 : 1);
        });
        return app.exec();
    } else if (argc > 1 && std::string(argv[1]) == "recv") {
        MockCanBackend receiver;
        receiver.open();
        VehicleState targetState;
        targetState.attachCanInterface(&receiver);
        QObject::connect(&targetState, &VehicleState::rpmChanged, [&]() {
            std::cout << "CROSS_PROCESS_RECV_RPM: " << targetState.rpm() << std::endl;
            QCoreApplication::quit();
        });
        QTimer::singleShot(3000, [&]() {
            std::cout << "CROSS_PROCESS_RECV_TIMEOUT" << std::endl;
            QCoreApplication::exit(1);
        });
        return app.exec();
    } else if (argc > 1 && std::string(argv[1]) == "send") {
        MockCanBackend sender;
        sender.open();
        QTimer::singleShot(200, [&]() {
            CanFrame f = NissanCan::encodeEngine(4200.0, 50.0, true);
            bool ok = sender.sendFrame(f);
            std::cout << "CROSS_PROCESS_SEND_OK: " << ok << std::endl;
            QTimer::singleShot(200, [&]() { QCoreApplication::quit(); });
        });
        return app.exec();
    } else if (argc > 1 && std::string(argv[1]) == "trace-can") {
        auto can = CanBackendFactory::create();
        VehicleState state;
        state.attachCanInterface(can.get());
        return runTrace(app, state);
    } else if (argc > 1 && std::string(argv[1]) == "trace-core") {
        VehicleModel mirror;
        CoreClient client(&mirror);

        Protocol::HelloInfo hello;
        hello.clientId = QStringLiteral("trace");
        hello.role = QString(Protocol::Role::CLUSTER);
        hello.displayId = QStringLiteral("cluster");
        hello.width = 1920;
        hello.height = 720;
        hello.wants = {QString(Protocol::Want::VEHICLE)};
        if (!client.start(hello)) return 2;
        return runTrace(app, mirror);
    }

    testCodecs();
    testVehicleState();
    testMockCanLoopback();

    std::cout << "[TEST] All Core unit tests completed successfully." << std::endl;
    return 0;
}
