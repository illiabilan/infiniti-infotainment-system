#include "CanBackendFactory.h"
#include "MockCanBackend.h"
#if defined(Q_OS_LINUX)
#include "SocketCanBackend.h"
#endif
#include <QDebug>

std::unique_ptr<CanInterface> CanBackendFactory::create(const QString &interfaceName)
{
#if defined(Q_OS_LINUX)
    {
        auto socketBackend = std::make_unique<SocketCanBackend>(interfaceName);
        if (socketBackend->open()) {
            return socketBackend;
        }
        qWarning().noquote() << QStringLiteral("%1 unavailable, falling back to multicast mock CAN").arg(interfaceName);
    }
#else
    Q_UNUSED(interfaceName)
#endif

    auto mockBackend = std::make_unique<MockCanBackend>();
    if (!mockBackend->open()) {
        qWarning() << "multicast mock CAN failed to open";
    }
    return mockBackend;
}
