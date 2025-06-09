
add_library(usbdrvlib STATIC
    usbdrv.c
    usbdrvasm.S
    oddebug.c
)

target_include_directories(usbdrvlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)