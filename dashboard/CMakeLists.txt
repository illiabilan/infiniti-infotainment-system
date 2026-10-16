cmake_minimum_required(VERSION 3.16)
project(SmartUiDashboard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Network Qml Svg)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
if(Qt6_VERSION VERSION_GREATER_EQUAL "6.7.0")
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(SmartUiDashboard
    main.cpp
    appbackend.h
    appbackend.cpp
    climatecontroller.h
    climatecontroller.cpp
    mediacontroller.h
    mediacontroller.cpp
    quicksettingscontroller.h
    quicksettingscontroller.cpp
    vehiclecontroller.h
    vehiclecontroller.cpp
    qml.qrc
)

target_include_directories(SmartUiDashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(SmartUiDashboard PRIVATE
    InfinitiCore
    InfinitiTheme
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::Qml
    Qt6::Svg
)
