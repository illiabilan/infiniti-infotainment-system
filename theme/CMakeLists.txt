cmake_minimum_required(VERSION 3.16)
project(InfinitiTheme LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2)

add_library(InfinitiTheme STATIC
    src/InfinitiTheme.cpp
    include/InfinitiTheme.h
    theme.qrc
)

target_include_directories(InfinitiTheme PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(InfinitiTheme PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
)
