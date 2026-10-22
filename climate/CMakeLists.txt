cmake_minimum_required(VERSION 3.16)
project(InfinitiClimate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Qml Svg WebSockets)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
if(Qt6_VERSION VERSION_GREATER_EQUAL "6.7.0")
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(InfinitiClimateApp
    src/main.cpp
    src/ClimateRibbonController.h
    src/ClimateRibbonController.cpp
    qml.qrc
)

qt_add_qml_module(InfinitiClimateApp
    URI InfinitiClimate
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/ClimateRibbonView.qml
)

target_include_directories(InfinitiClimateApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(InfinitiClimateApp PRIVATE
    InfinitiCoreClient
    InfinitiTheme
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::Qml
    Qt6::Svg
)
