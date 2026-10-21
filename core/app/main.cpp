#include <QCoreApplication>
#include <QDebug>

#include "AaSessionState.h"
#include "CanBackendFactory.h"
#include "CoreEndpointConfig.h"
#include "CoreServer.h"
#include "VehicleState.h"

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

    // Declaration order matters: backend, vehicle, session, then server.
    // Destruction runs in reverse, so the server goes before the vehicle.
    auto canBackend = CanBackendFactory::create();

    VehicleState vehicleState;
    vehicleState.attachCanInterface(canBackend.get());

    AaSessionState aaSession;
    CoreServer server(&vehicleState, &aaSession);

    if (!server.listen(*port)) {
        qCritical().noquote() << QStringLiteral("listen on 127.0.0.1:%1 failed: %2").arg(*port).arg(server.errorString());
        return 1;
    }

    qInfo().noquote() << QStringLiteral("core: listening 127.0.0.1:%1 pid=%2")
                             .arg(*port)
                             .arg(QCoreApplication::applicationPid());

    return app.exec();
}
