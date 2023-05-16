cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)
set(TARGET ${TARGET} "rich_app")
message(STATUS "Selected target is ${TARGET}")
option(ENARX_RA "Add the depencies for the Enarx riach aplication" OFF)
message(STATUS "ENARX_RA option: ${ENARX_RA}")


add_compile_definitions(LOG_USE_COLOR)
if (${TARGET} STREQUAL "enarx")
    set(CMAKE_C_COMPILER "/home/debian/wasi-sdk/build/wasi-sdk-17.0/bin/clang")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -fPIC -target wasm32-wasi")
    add_compile_definitions(TRUSTED_APP)
    add_compile_definitions(Enarx)
elseif(${TARGET} STREQUAL "rich_app")
    message(STATUS "ENARX_RA option: ${ENARX_RA}")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build-scripts/CMakeModules/")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pthread")
endif()


# set the project name and version
set (PROJECT_NAME "i2nsf_cfgipsec")
project(${PROJECT_NAME} VERSION 1.0)

include(GNUInstallDirs)
include(CheckFunctionExists)
include(CheckIncludeFile)




if (${TARGET} STREQUAL "enarx")
    include("cmd/i2nsf_enarx/CMakeLists.txt")
elseif(${TARGET} STREQUAL "rich_app")
    include("cmd/i2nsf_ra/CMakeLists.txt")
endif()