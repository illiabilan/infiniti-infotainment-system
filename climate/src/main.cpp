#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "ClimateRibbonController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Infiniti G35 Climate Ribbon");
    app.setOrganizationName("Infiniti HMI");

    QQuickStyle::setStyle("Basic");

    QQmlApplicationEngine engine;
    ClimateRibbonController controller;

    engine.rootContext()->setContextProperty("climateController", &controller);

    const QUrl url(QStringLiteral("qrc:/qt/qml/InfinitiClimate/qml/Main.qml"));
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
