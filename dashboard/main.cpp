#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QUrl>
#include <QDebug>

#include <memory>

#include "CoreClient.h"
#include "InfinitiTheme.h"
#include "VehicleModel.h"

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

    auto vehicleState = std::make_unique<VehicleModel>();

    CoreClient coreClient(vehicleState.get());
    Protocol::HelloInfo hello;
    hello.clientId = QStringLiteral("dash");
    hello.role = QString(Protocol::Role::DASH);
    hello.displayId = QStringLiteral("main");
    hello.width = 1920;
    hello.height = 1080;
    hello.wants = {QString(Protocol::Want::VEHICLE)};
    if (!coreClient.start(hello)) {
        return 2;
    }

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
