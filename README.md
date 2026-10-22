# Infiniti G35 Multi-Display Infotainment System

Triple-display automotive HMI in C++20 and Qt 6 (QML), driven by a shared CAN telemetry layer. Target: Radxa ROCK 5B+ (RK3588, Linux/Wayland, SocketCAN). Dev host: macOS or Linux with mock CAN fallback.

## Screens

### Cluster (1920x720)

![Cluster](docs/images/cluster.png)

Digital instrument cluster: speed and RPM gauges, coolant and fuel bars, gear indicator, telltales, trip computer, TPMS, and themes. Details: [cluster/README.md](cluster/README.md). Run: `just run-cluster`.

### Head Unit (1920x1080)

![Head unit](docs/images/dashboard.png)

Smart UI home screen: clock, quick controls, weather, music, vehicle controls, energy, and settings with light and dark themes. Details: [dashboard/README.md](dashboard/README.md). Run: `just run-dash`.

### Climate Ribbon (1920x480)

![Climate ribbon](docs/images/climate-ribbon.png)

Dual-zone climate strip: driver and passenger setpoints, HVAC status with direction and fan icons, A/C, AUTO, RECIRC, and ECO chips, outside temp, and clock. Details: [climate/README.md](climate/README.md). Run: `just run-clima`.

### Emulator

Standalone ECU test bench driving all screens over CAN. Run: `just run-emu`, or everything at once with `just run-all`.

## CAN layer

`core/` holds the shared `VehicleState`, Nissan CAN codec (`0x1F9`, `0x280`, `0x54C`, ...), `SocketCanBackend` for `vcan0`/`can0`, and `MockCanBackend` multicast fallback when no CAN interface exists. The Core process (`core/app`, run with `just run-core`) owns CAN and the only `VehicleState` of the head unit. Display apps connect to `ws://127.0.0.1:<port from configs/core.ini>` and mirror the vehicle state. The emulator drives Core over CAN, so start Core first or in any order, since clients retry every second.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
./build/core/core_test
ctest --test-dir build --output-on-failure
```

Android Auto USB backend (optional, `AA_WITH_AASDK`, default OFF). Core then also builds the aasdk AOA adapter from the `third_party/aasdk` submodule (pinned commit `9bf6adf`). Use the Ninja generator.

```bash
git submodule update --init third_party/aasdk
cmake -S . -B build/aa-on -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAA_WITH_AASDK=ON -DCMAKE_PREFIX_PATH="<boost-1.85 prefix>;<other prefixes>"
cmake --build build/aa-on -j
```

Dependencies: libusb 1.0, OpenSSL, protobuf, abseil and Boost 1.85. Boost 1.87 removed `boost::asio::io_service`, which aasdk uses. macOS (Homebrew): `boost@1.85` is keg-only, pass `-DCMAKE_PREFIX_PATH="/opt/homebrew/opt/boost@1.85;/opt/homebrew"`. Homebrew disabled that formula on 2026-04-05, so if `brew install boost@1.85` refuses, build Boost 1.85.0 from source as on Debian. Debian (not yet verified on ROCK 5B+): build Boost 1.85.0 from source into `/opt/boost-1.85` (`./bootstrap.sh --prefix=/opt/boost-1.85 --with-libraries=system,log && ./b2 -j"$(nproc)" install`) and pass `-DCMAKE_PREFIX_PATH=/opt/boost-1.85`. With `AA_WITH_AASDK=OFF`, Core logs `aa-usb: USB backend not built` and runs without an AOA backend.

## Credits

- Cluster UI builds on the open-source [APEX Horizon AMT cluster](https://github.com/skrehanahamed/Apex_MidEnd_Cluster) community project.
- Head unit UI builds on the open-source [professional vehicle dashboard](https://github.com/cppqtdev/professional-vehicle-dashboard) community project.
