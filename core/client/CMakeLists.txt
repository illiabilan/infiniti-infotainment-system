add_library(InfinitiCoreClient STATIC
    CoreClient.h
    CoreClient.cpp
)

target_include_directories(InfinitiCoreClient PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(InfinitiCoreClient PUBLIC
    InfinitiProtocol
    Qt6::WebSockets
)
