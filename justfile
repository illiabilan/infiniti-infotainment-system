default: build

# Incremental build using the preconfigured build tree
build:
    cmake --build build -j

# Single target compilation
build-target target="ApexClusterApp":
    cmake --build build --target {{target}}

# Clean configuration and build on macOS host
configure:
    cmake -B build -S . -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
    cmake --build build -j

# Run Core process (owns CAN and VehicleState, serves ws://127.0.0.1)
run-core:
    ./build/core/app/InfinitiCoreApp

# Run climate ribbon application
run-clima:
    ./build/climate/InfinitiClimateApp

# Run cluster display application
run-cluster:
    ./build/cluster/ApexClusterApp

# Run dashboard head unit application
run-dash:
    ./build/dashboard/SmartUiDashboard

# Run all 3 display applications concurrently in one terminal session
run-all:
    ./build/core/app/InfinitiCoreApp & ./build/cluster/ApexClusterApp & ./build/dashboard/SmartUiDashboard & ./build/climate/InfinitiClimateApp & wait

# Run standalone telemetry emulator bench
run-emu:
    ./build/emulator/InfinitiEmulatorApp

# Test clima
test-clima:
    ./build/core/app/InfinitiCoreApp & ./build/emulator/InfinitiEmulatorApp & ./build/climate/InfinitiClimateApp
