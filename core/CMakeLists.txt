cmake_minimum_required(VERSION 3.20)
project(InfinitiCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network WebSockets Test)

add_subdirectory(protocol)
add_subdirectory(aa)

add_library(InfinitiCore STATIC
    include/CanBackendFactory.h
    src/CanBackendFactory.cpp
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
    InfinitiProtocol
    Qt6::Core
    Qt6::Network
)

target_compile_features(InfinitiCore PUBLIC cxx_std_20)

add_subdirectory(client)
add_subdirectory(app)

# Verification / Unit test executable
add_executable(core_test
    tests/CoreTest.cpp
)

target_link_libraries(core_test PRIVATE
    InfinitiCore
    InfinitiCoreClient
    Qt6::Core
    Qt6::Network
)

# Protocol tests (CL-2)
qt_add_executable(core_protocol_test
    tests/CoreProtocolTest.cpp
)

target_link_libraries(core_protocol_test PRIVATE
    InfinitiCoreServer
    Qt6::Test
)

# Z1 AOA replay (AA-Z1-T3)
qt_add_executable(core_aa_usb_test
    aa/tests/AoaSwitcherReplayTest.cpp
)

target_link_libraries(core_aa_usb_test PRIVATE
    CoreAa
    Qt6::Test
)

target_compile_definitions(core_aa_usb_test PRIVATE
    AOA_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/aa/tests/fixtures"
)

add_test(NAME core_test COMMAND core_test)
add_test(NAME core_protocol_test COMMAND core_protocol_test)
add_test(NAME core_aa_usb_test COMMAND core_aa_usb_test)
