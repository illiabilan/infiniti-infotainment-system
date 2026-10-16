#include "InfinitiTheme.h"

#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>

static void initResources()
{
    Q_INIT_RESOURCE(theme);
}

namespace InfinitiTheme {

void setup(QQmlApplicationEngine &engine)
{
    initResources();

    engine.addImportPath(QStringLiteral("qrc:/qml"));

    qmlRegisterSingletonType(
        QUrl(QStringLiteral("qrc:/qml/App/Theme/Theme.qml")),
        "App.Theme", 1, 0, "Theme");
    qmlRegisterSingletonType(
        QUrl(QStringLiteral("qrc:/qml/App/Icons/Icons.qml")),
        "App.Icons", 1, 0, "Icons");
}

} // namespace InfinitiTheme
