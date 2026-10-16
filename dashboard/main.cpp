#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QUrl>
#include <QDebug>

#include <memory>

#include "CanInterface.h"
#include "MockCanBackend.h"
#if defined(Q_OS_LINUX)
#include "SocketCanBackend.h"
#endif
#include "InfinitiTheme.h"
#include "VehicleState.h"

#include "appbackend.h"
#include "climatecontroller.h"
#include "mediacontroller.h"
#include "quicksettingscontroller.h"
#include "vehiclecontroller.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    std::unique_ptr<CanInterface> canBackend;
#if defined(Q_OS_LINUX)
    {
        auto socketBackend = std::make_unique<SocketCanBackend>("vcan0");
        if (socketBackend->open()) {
            canBackend = std::move(socketBackend);
        } else {
            qWarning() << "vcan0 unavailable, falling back to multicast mock CAN";
            canBackend = std::make_unique<MockCanBackend>();
            canBackend->open();
        }
    }
#else
    canBackend = std::make_unique<MockCanBackend>();
    canBackend->open();
#endif

    auto vehicleState = std::make_unique<VehicleState>();
    vehicleState->attachCanInterface(canBackend.get());

    QQmlApplicationEngine engine;
    InfinitiTheme::setup(engine);

    AppBackend appBackend;
    QuickSettingsController quickSettings;
    MediaController mediaController;
    VehicleController vehicleController;
    ClimateController climateController;

    vehicleController.attachVehicleState(vehicleState.get());
    climateController.attachVehicleState(vehicleState.get());
    mediaController.attachVehicleState(vehicleState.get());

    engine.rootContext()->setContextProperty(QStringLiteral("appBackend"), &appBackend);
    engine.rootContext()->setContextProperty(QStringLiteral("quickControls"), &quickSettings);
    engine.rootContext()->setContextProperty(QStringLiteral("mediaControls"), &mediaController);
    engine.rootContext()->setContextProperty(QStringLiteral("vehicleControls"), &vehicleController);
    engine.rootContext()->setContextProperty(QStringLiteral("climateControls"), &climateController);
    engine.rootContext()->setContextProperty(QStringLiteral("vehicleState"), vehicleState.get());

    const QUrl url(QStringLiteral("qrc:/main.qml"));
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
