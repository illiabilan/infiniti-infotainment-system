cmake_minimum_required(VERSION 3.20)
project(InfinitiInfotainmentSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Optional Android Auto USB backend from the aasdk submodule (AA-Z1)
option(AA_WITH_AASDK "Build the aasdk AOA backend for Core (needs libusb, OpenSSL, protobuf, abseil, Boost 1.85)" OFF)

if(AA_WITH_AASDK)
    # aasdk 9bf6adf uses boost::asio::io_service, which Boost 1.87 removed
    find_package(Boost 1.85 REQUIRED CONFIG)
    if(Boost_VERSION VERSION_GREATER_EQUAL 1.87)
        message(FATAL_ERROR "AA_WITH_AASDK needs Boost below 1.87, found ${Boost_VERSION}")
    endif()

    set(AASDK_TEST OFF CACHE BOOL "Build AASDK unit tests")
    set(SKIP_BUILD_PROTOBUF ON CACHE BOOL "Skip building protobuf, use system-installed version")
    set(SKIP_BUILD_ABSL ON CACHE BOOL "Skip building Abseil, use system-installed version")
    # aasdk cmake_modules/gitversion.cmake declares cmake_minimum_required(VERSION 3.0.0), which CMake 4 rejects
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Lowest policy version accepted from subprojects")
    add_subdirectory(third_party/aasdk EXCLUDE_FROM_ALL)
endif()

# Shared Core CAN and Telemetry Library
add_subdirectory(core)

# Shared Theme & Design System Library
add_subdirectory(theme)

# Standalone Telemetry Emulator Bench
add_subdirectory(emulator)

# Cluster Display (1920x720)
add_subdirectory(cluster)

# Center Dashboard / Head Unit Display (1920x1080)
add_subdirectory(dashboard)

# Climate Ribbon Display (1920x480)
add_subdirectory(climate)
