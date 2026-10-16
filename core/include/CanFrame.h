#pragma once

#include <cstdint>
#include <cstring>

#pragma pack(push, 1)
struct CanFrame {
    uint32_t canId{0};
    uint8_t dlc{0};
    uint8_t data[8]{0, 0, 0, 0, 0, 0, 0, 0};

    static CanFrame make(uint32_t id, uint8_t len, const uint8_t* bytes = nullptr) {
        CanFrame f;
        f.canId = id;
        f.dlc = (len > 8) ? 8 : len;
        if (bytes && f.dlc > 0) {
            std::memcpy(f.data, bytes, f.dlc);
        }
        return f;
    }
};
#pragma pack(pop)
