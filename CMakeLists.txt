cmake_minimum_required(VERSION 3.10)
project(corewar)

set(CMAKE_CXX_STANDARD 20)

#***********************************************************************************************************************
# SET PATH
# GaLib - Header
set(GALIB_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../galib")
# GaLib Library
set(GALIB_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../galib/cmake-build-release/libgalib.a")
# PMARS
set(PMARS_PATH "../../pmars-0.9.4/src/pmars")
#***********************************************************************************************************************


# GaLib - Header
if(EXISTS "${GALIB_HEADER_PATH}")
    get_filename_component(ABSOLUTE_HEADER_PATH "${GALIB_HEADER_PATH}" ABSOLUTE)
    message(STATUS "Abs-Path of GaLib-Library Header-Files: ${ABSOLUTE_HEADER_PATH}")
    include_directories("${ABSOLUTE_HEADER_PATH}")
else()
    message(FATAL_ERROR "GaLib header path does not exist: ${GALIB_HEADER_PATH}")
endif()

add_executable(corewar main.cpp)

# GaLib Library
if(EXISTS "${GALIB_LIBRARY_PATH}")
    get_filename_component(ABSOLUTE_LIBRARY_PATH "${GALIB_LIBRARY_PATH}" ABSOLUTE)
    message(STATUS "Abs-Path of GaLib-Library: ${ABSOLUTE_LIBRARY_PATH}")
    target_link_libraries(corewar "${ABSOLUTE_LIBRARY_PATH}")
else()
    message(FATAL_ERROR "GaLib library file does not exist: ${GALIB_LIBRARY_PATH}")
endif()

# PMARS
if(EXISTS "${PMARS_PATH}")
    message(STATUS "PMARS path exists: ${PMARS_PATH}")
else()
    message(FATAL_ERROR "PMARS path does not exist: ${PMARS_PATH}")
endif()

# Create Config (config.h)
set(CONFIG_H "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in")
if(EXISTS "${CONFIG_H}")
    configure_file(config.h.in config.h)
    target_include_directories(corewar PRIVATE ${CMAKE_BINARY_DIR})
else()
    message(FATAL_ERROR "config.h.in does not exist, skipping configure_file step.")
endif()
