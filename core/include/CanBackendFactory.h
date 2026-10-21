#pragma once

#include "CanInterface.h"
#include <QString>
#include <memory>

// Single place that selects the CAN backend: SocketCAN on Linux with a
// multicast mock fallback, mock only on other platforms.
class CanBackendFactory
{
public:
    static constexpr const char *DEFAULT_INTERFACE = "vcan0";

    static std::unique_ptr<CanInterface> create(const QString &interfaceName = QString::fromLatin1(DEFAULT_INTERFACE));
};
