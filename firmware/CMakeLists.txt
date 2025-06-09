cmake_minimum_required(VERSION 4.0)

project("USBaspv2" LANGUAGES C ASM)

set(MCU   atmega8)
set(CMAKE_C_FLAGS "-Wall -O2 -mmcu=${MCU} --verbose")
set(CMAKE_ASM_FLAGS "-mmcu=${MCU}")


add_executable(${PROJECT_NAME} usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c isp.c clock.c tpi.S main.c)

target_include_directories( ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/usbdrv 
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMENT "Generating HEX file"
)