#include <QCoreApplication>
#include <QDebug>

#include "AaSessionState.h"
#include "CanBackendFactory.h"
#include "CoreEndpointConfig.h"
#include "CoreServer.h"
#include "VehicleState.h"

#ifdef AA_WITH_AASDK
#include "AasdkUsbPort.h"
#include "AoaSwitcher.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Infiniti Core");
    app.setOrganizationName("Infiniti HMI");

    QString configError;
    const std::optional<quint16> port = CoreEndpointConfig::loadPort(&configError);
    if (!port) {
        qCritical().noquote() << configError;
        return 2;
    }

    // Declaration order matters: backend, vehicle, session, USB port and switcher, then server.
    // Destruction runs in reverse: server first, then the switcher before the port it uses, then the vehicle.
    auto canBackend = CanBackendFactory::create();

    VehicleState vehicleState;
    vehicleState.attachCanInterface(canBackend.get());

    AaSessionState aaSession;
#ifdef AA_WITH_AASDK
    AasdkUsbPort usbPort;
    AoaSwitcher aoaSwitcher(&usbPort);
#endif
    CoreServer server(&vehicleState, &aaSession);

    if (!server.listen(*port)) {
        qCritical().noquote() << QStringLiteral("listen on 127.0.0.1:%1 failed: %2").arg(*port).arg(server.errorString());
        return 1;
    }

    qInfo().noquote() << QStringLiteral("core: listening 127.0.0.1:%1 pid=%2")
                             .arg(*port)
                             .arg(QCoreApplication::applicationPid());

#ifdef AA_WITH_AASDK
    // AoaSwitcher logs a failed start and Core keeps running
    aoaSwitcher.start();
#else
    qInfo().noquote() << QStringLiteral("aa-usb: USB backend not built (AA_WITH_AASDK=OFF), core runs without an AOA backend");
#endif

    return app.exec();
}
