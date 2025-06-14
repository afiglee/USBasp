cmake_minimum_required(VERSION 4.0)

project("flashup" LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-Wall -O2")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFileCXX)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
 execute_process(
        COMMAND brew --prefix
        RESULT_VARIABLE BREW_RESULT
        OUTPUT_VARIABLE BREW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(BREW_RESULT EQUAL 0)
        set(CMAKE_PREFIX_PATH "${BREW_PREFIX}" ${CMAKE_PREFIX_PATH})
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_PREFIX_PATH}/include")
endif()
find_library(HAVE_LIBUSB_1_0 NAMES usb-1.0 libusb-1.0)
if(HAVE_LIBUSB_1_0)
    message(STATUS "Found libusb-1.0: ${HAVE_LIBUSB_1_0}")
endif()

CHECK_INCLUDE_FILE_CXX(libusb.h HAVE_LIBUSB_H)
CHECK_INCLUDE_FILE_CXX(libusb-1.0/libusb.h HAVE_LIBUSB_1_0_LIBUSB_H)

if(NOT HAVE_LIBUSB_1_0)
    message(FATAL_ERROR "libusb-1.0 not found, please install it")
endif()


if(HAVE_LIBUSB_1_0_LIBUSB_H)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_LIBUSB_1_0_LIBUSB_H")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_LIBUSB_1_0")
add_executable(${PROJECT_NAME} flashup.cpp usb.cpp log.cpp usbasp.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${HAVE_LIBUSB_1_0})
