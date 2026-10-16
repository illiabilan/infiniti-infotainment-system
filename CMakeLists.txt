cmake_minimum_required(VERSION 3.20)
project(InfinitiInfotainmentSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
