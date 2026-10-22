add_library(InfinitiAa STATIC
    session/AaSessionState.h
    session/AaSessionState.cpp
)

target_include_directories(InfinitiAa PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/session>
)

target_link_libraries(InfinitiAa PUBLIC
    Qt6::Core
)

target_compile_features(InfinitiAa PUBLIC cxx_std_20)
