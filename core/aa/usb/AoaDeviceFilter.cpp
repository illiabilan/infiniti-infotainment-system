#include "AoaDeviceFilter.h"

#include <algorithm>
#include <array>

namespace {

constexpr quint16 ACCESSORY_VENDOR_ID = 0x18d1;
constexpr quint16 ACCESSORY_PRODUCT_FIRST = 0x2d00;
constexpr quint16 ACCESSORY_PRODUCT_LAST = 0x2d05;

constexpr quint8 CLASS_PER_INTERFACE = 0x00;
constexpr quint8 CLASS_VENDOR_SPECIFIC = 0xff;
constexpr quint8 CLASS_WIRELESS = 0xe0;

// Vendor and interface tables follow LIVI native/livi-helperd/crates/livi-aa/src/usb.rs:19-28 (GPL-3.0-or-later)
constexpr std::array<quint16, 16> CANDIDATE_VENDOR_IDS{
    0x0489, 0x04dd, 0x04e8, 0x0b05, 0x0bb4, 0x0e8d, 0x0fce, 0x1004,
    0x109b, 0x12d1, 0x17ef, 0x18d1, 0x19d2, 0x22b8, 0x2717, 0x2a70
};

constexpr std::array<quint8, 10> NON_PHONE_INTERFACE_CLASSES{
    0x01, 0x02, 0x03, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0e, 0xe0
};

bool isNonPhoneInterface(quint8 interfaceClass)
{
    return std::find(NON_PHONE_INTERFACE_CLASSES.begin(), NON_PHONE_INTERFACE_CLASSES.end(), interfaceClass)
           != NON_PHONE_INTERFACE_CLASSES.end();
}

}

namespace AoaDeviceFilter {

bool isAccessory(const UsbDeviceInfo &info)
{
    return info.vendorId == ACCESSORY_VENDOR_ID
           && info.productId >= ACCESSORY_PRODUCT_FIRST
           && info.productId <= ACCESSORY_PRODUCT_LAST;
}

bool isCandidate(const UsbDeviceInfo &info)
{
    if (std::find(CANDIDATE_VENDOR_IDS.begin(), CANDIDATE_VENDOR_IDS.end(), info.vendorId)
        == CANDIDATE_VENDOR_IDS.end()) {
        return false;
    }
    if (info.deviceClass != CLASS_PER_INTERFACE && info.deviceClass != CLASS_VENDOR_SPECIFIC) {
        return false;
    }
    if (info.interfaceClasses.contains(CLASS_WIRELESS)) {
        return false;
    }
    if (info.interfaceClasses.isEmpty()) {
        return true;
    }
    return std::any_of(info.interfaceClasses.begin(), info.interfaceClasses.end(),
                       [](quint8 interfaceClass) { return !isNonPhoneInterface(interfaceClass); });
}

}
