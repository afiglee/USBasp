cmake_minimum_required(VERSION 4.0)

project("USBaspv2" LANGUAGES C ASM)

set(MCU   atmega8)
set(CMAKE_C_FLAGS "-Wall -O2 -mmcu=${MCU}")
set(CMAKE_ASM_FLAGS "-Wall -O2 -mmcu=${MCU}")

add_executable(${PROJECT_NAME} isp.c clock.c tpi.S main.c)

target_include_directories( ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/usbdrv 
)
target_link_libraries(${PROJECT_NAME} PRIVATE usbdrvlib)


add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMENT "Generating HEX file"
)

add_subdirectory(usbdrv)
# Add a custom target to flash the device
add_custom_target(flash
    COMMAND avrdude -c usbasp -p ${MCU} -U flash:w:${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    DEPENDS ${PROJECT_NAME}
    COMMENT "Flashing the device with avrdude"
)