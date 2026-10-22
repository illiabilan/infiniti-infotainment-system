add_library(InfinitiCoreServer STATIC
    ClientConnection.h
    ClientConnection.cpp
    CoreServer.h
    CoreServer.cpp
)

target_include_directories(InfinitiCoreServer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(InfinitiCoreServer PUBLIC
    InfinitiProtocol
    InfinitiAa
    Qt6::WebSockets
)

qt_add_executable(InfinitiCoreApp
    main.cpp
)

target_link_libraries(InfinitiCoreApp PRIVATE
    InfinitiCoreServer
    InfinitiCore
)
