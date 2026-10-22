#pragma once

#include "UsbDeviceInfo.h"

// Pure classification of attached USB devices for Android Open Accessory.
namespace AoaDeviceFilter {

// A device already in accessory mode
bool isAccessory(const UsbDeviceInfo &info);

// A phone in normal USB mode worth switching to accessory mode.
// It does not exclude accessories, so callers test isAccessory first.
bool isCandidate(const UsbDeviceInfo &info);

}
