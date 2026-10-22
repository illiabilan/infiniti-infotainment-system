add_library(CoreAa STATIC
    session/AaSessionState.h
    session/AaSessionState.cpp
    usb/UsbDeviceInfo.h
    usb/AoaDeviceFilter.h
    usb/AoaDeviceFilter.cpp
    usb/AoaMarkers.h
    usb/AoaUsbPort.h
    usb/AoaSwitcher.h
    usb/AoaSwitcher.cpp
)

target_include_directories(CoreAa PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/session>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/usb>
)

target_link_libraries(CoreAa PUBLIC
    Qt6::Core
)

target_compile_features(CoreAa PUBLIC cxx_std_20)

# aasdk adapter, separate target so CoreAa and its tests never link aasdk or libusb
if(AA_WITH_AASDK)
    if(NOT LIBUSB_1_INCLUDE_DIR)
        message(FATAL_ERROR "AA_WITH_AASDK: libusb headers not found (LIBUSB_1_INCLUDE_DIR from aasdk)")
    endif()

    add_library(CoreAaAasdk STATIC
        usb/AasdkUsbPort.h
        usb/AasdkUsbPort.cpp
    )

    # aasdk exposes its own headers only through directory-scoped include_directories()
    target_include_directories(CoreAaAasdk PRIVATE
        ${CMAKE_SOURCE_DIR}/third_party/aasdk/include
        ${LIBUSB_1_INCLUDE_DIR}
    )

    target_link_libraries(CoreAaAasdk
        PUBLIC
            CoreAa
        PRIVATE
            aasdk
            Boost::headers
            ${LIBUSB_1_LIBRARY}
    )
endif()
