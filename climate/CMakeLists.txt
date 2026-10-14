cmake_minimum_required(VERSION 3.16)
project(InfinitiClimate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Qml)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
if(Qt6_VERSION VERSION_GREATER_EQUAL "6.7.0")
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(InfinitiClimateApp
    src/main.cpp
    src/ClimateRibbonController.h
    src/ClimateRibbonController.cpp
)

qt_add_qml_module(InfinitiClimateApp
    URI InfinitiClimate
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        qml/Main.qml
        qml/ClimateRibbonView.qml
        qml/components/TempControl.qml
        qml/components/FanControl.qml
        qml/components/AirflowControl.qml
        qml/components/ClimateButton.qml
        qml/components/SeatHeaterButton.qml
        qml/components/ClockDisplay.qml
)

target_link_libraries(InfinitiClimateApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Qml
)
