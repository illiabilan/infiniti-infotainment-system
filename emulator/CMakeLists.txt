cmake_minimum_required(VERSION 3.20)
project(InfinitiEmulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Qml Network)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
if(Qt6_VERSION VERSION_GREATER_EQUAL "6.7.0")
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(InfinitiEmulatorApp
    src/main.cpp
    src/EmulatorController.h
    src/EmulatorController.cpp
)

qt_add_qml_module(InfinitiEmulatorApp
    URI InfinitiEmulator
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        qml/Main.qml
        qml/EcuSimulatorPanel.qml
)

target_include_directories(InfinitiEmulatorApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(InfinitiEmulatorApp PRIVATE
    InfinitiCore
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Qml
    Qt6::Network
)
