#include "CoreEndpointConfig.h"
#include <QFileInfo>
#include <QSettings>

namespace CoreEndpointConfig {

QString configPath()
{
    const QString fromEnvironment = qEnvironmentVariable("INFINITI_CORE_CONFIG");
    if (!fromEnvironment.isEmpty()) {
        return fromEnvironment;
    }
    return QStringLiteral(INFINITI_CORE_CONFIG_PATH);
}

std::optional<quint16> loadPort(QString *errorText)
{
    const QString path = configPath();

    if (!QFileInfo::exists(path)) {
        *errorText = QStringLiteral("Core config not found: %1").arg(path);
        return std::nullopt;
    }

    const QSettings settings(path, QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        *errorText = QStringLiteral("Core config unreadable: %1").arg(path);
        return std::nullopt;
    }

    bool ok = false;
    const uint port = settings.value(QStringLiteral("server/port")).toUInt(&ok);
    if (!ok || port < 1 || port > 65535) {
        *errorText = QStringLiteral("server/port missing or invalid in %1").arg(path);
        return std::nullopt;
    }

    return static_cast<quint16>(port);
}

QUrl serverUrl(quint16 port)
{
    return QUrl(QStringLiteral("ws://127.0.0.1:%1").arg(port));
}

} // namespace CoreEndpointConfig
