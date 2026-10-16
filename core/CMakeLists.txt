cmake_minimum_required(VERSION 3.20)
project(InfinitiCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(InfinitiCore STATIC
    include/CanFrame.h
    include/NissanCanDefinitions.h
    include/CanInterface.h
    include/SocketCanBackend.h
    src/SocketCanBackend.cpp
    include/MockCanBackend.h
    src/MockCanBackend.cpp
    include/VehicleState.h
    src/VehicleState.cpp
)

target_include_directories(InfinitiCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(InfinitiCore PUBLIC
    Qt6::Core
    Qt6::Network
)

target_compile_features(InfinitiCore PUBLIC cxx_std_20)

# Verification / Unit test executable
add_executable(core_test
    tests/CoreTest.cpp
)

target_link_libraries(core_test PRIVATE
    InfinitiCore
    Qt6::Core
    Qt6::Network
)
