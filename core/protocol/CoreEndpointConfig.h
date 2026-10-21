#pragma once

#include <QString>
#include <QUrl>
#include <optional>

// Core WebSocket endpoint settings from configs/core.ini (CL-2).
// The host is fixed to 127.0.0.1 and is not configurable.
namespace CoreEndpointConfig {

// INFINITI_CORE_CONFIG if set, otherwise the compile-time path
QString configPath();

// Returns std::nullopt with *errorText set if the file is missing or invalid
std::optional<quint16> loadPort(QString *errorText);

QUrl serverUrl(quint16 port);

} // namespace CoreEndpointConfig
