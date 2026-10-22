add_library(InfinitiProtocol STATIC
    VehicleModel.h
    VehicleModel.cpp
    VehicleJson.h
    VehicleJson.cpp
    VehicleChangeWatcher.h
    VehicleChangeWatcher.cpp
    Protocol.h
    Protocol.cpp
    CoreEndpointConfig.h
    CoreEndpointConfig.cpp
)

target_include_directories(InfinitiProtocol PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(InfinitiProtocol PUBLIC
    Qt6::Core
)

target_compile_definitions(InfinitiProtocol PRIVATE
    INFINITI_CORE_CONFIG_PATH="${CMAKE_SOURCE_DIR}/configs/core.ini"
)

target_compile_features(InfinitiProtocol PUBLIC cxx_std_20)
