#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTimer>
#include <memory>

#include "CanInterface.h"
#include "MockCanBackend.h"
#if defined(Q_OS_LINUX)
#include "SocketCanBackend.h"
#endif
#include "VehicleState.h"
#include "EmulatorController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Infiniti G35 ECU Telemetry Bench");
    app.setOrganizationName("Infiniti HMI");

    QQuickStyle::setStyle("Basic");

    std::unique_ptr<CanInterface> canBackend;
#if defined(Q_OS_LINUX)
    canBackend = std::make_unique<SocketCanBackend>("vcan0");
#else
    canBackend = std::make_unique<MockCanBackend>();
#endif
    canBackend->open();

    auto vehicleState = std::make_unique<VehicleState>();
    vehicleState->attachCanInterface(canBackend.get());

    auto controller = std::make_unique<EmulatorController>(vehicleState.get());

    vehicleState->broadcastAll();

    QTimer heartbeatTimer;
    QObject::connect(&heartbeatTimer, &QTimer::timeout, [&vehicleState]() {
        vehicleState->broadcastAll();
    });
    heartbeatTimer.start(100);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", controller.get());
    engine.rootContext()->setContextProperty("vehicleState", vehicleState.get());

    const QUrl url(QStringLiteral("qrc:/qt/qml/InfinitiEmulator/qml/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
