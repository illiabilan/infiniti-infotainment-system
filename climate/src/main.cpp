#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QDebug>
#include <memory>

#include "CoreClient.h"
#include "InfinitiTheme.h"
#include "VehicleModel.h"
#include "ClimateRibbonController.h"

static constexpr int CLIMATE_SCREEN_WIDTH = 1280;
static constexpr int CLIMATE_SCREEN_HEIGHT = 160;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Infiniti G35 Climate Ribbon");
    app.setOrganizationName("Infiniti HMI");

    auto vehicleState = std::make_unique<VehicleModel>();

    CoreClient coreClient(vehicleState.get());
    Protocol::HelloInfo hello;
    hello.clientId = QStringLiteral("climate");
    hello.role = QString(Protocol::Role::CLIMATE);
    hello.displayId = QStringLiteral("climate");
    hello.width = 1920;
    hello.height = 480;
    hello.wants = {QString(Protocol::Want::VEHICLE)};
    if (!coreClient.start(hello)) {
        return 2;
    }

    QQmlApplicationEngine engine;
    InfinitiTheme::setup(engine);

    ClimateRibbonController controller;
    controller.attachVehicleState(vehicleState.get());

    engine.rootContext()->setContextProperty(QStringLiteral("climateController"), &controller);
    engine.rootContext()->setContextProperty(QStringLiteral("vehicleState"), vehicleState.get());

    const QUrl url(QStringLiteral("qrc:/qt/qml/InfinitiClimate/qml/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [&url](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == url)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    for (QObject *obj : engine.rootObjects()) {
        if (QQuickWindow *win = qobject_cast<QQuickWindow *>(obj)) {
            win->resize(CLIMATE_SCREEN_WIDTH, CLIMATE_SCREEN_HEIGHT);
            win->setMinimumSize(QSize(CLIMATE_SCREEN_WIDTH, CLIMATE_SCREEN_HEIGHT));
            win->setMaximumSize(QSize(CLIMATE_SCREEN_WIDTH, CLIMATE_SCREEN_HEIGHT));
            win->setFlags(win->flags() | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint);
            win->show();
        }
    }

    return app.exec();
}
