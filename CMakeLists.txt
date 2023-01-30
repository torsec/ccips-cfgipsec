cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE Debug)
set(TARGET ${TARGET} "enarx")
option(USE_SYSREPO OFF)
option(WASI_TARGET ON)
message(STATUS "Selected target is ${TARGET}")



if (WASI_TARGET)
    # set(CMAKE_SKIP_BUILD_RPATH TRUE)
    # set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)  
    # set(WASI_SDK_PATH  "/home/debian/wasi-sdk/build/wasi-sdk-17.0")
    # # set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra --target=wasm32-wasi -Oz --sysroot=${WASI_SDK_PATH}/share/wasi-sysroot")
    # # set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra --target=wasm32-wasi -Oz --sysroot=${WASI_SDK_PATH}/share/wasi-sysroot")
    # set(CMAKE_C_FLAGS "-g -Wall -Wextra")
    set(CMAKE_C_COMPILER "/home/debian/wasi-sdk/build/wasi-sdk-17.0/bin/clang")
    # set(CMAKE_CXX_COMPILER "/usr/bin/clang")
    # set(WASI_LIBC_PATH "/home/debian/wasi-libc/build/")
    # Link the wasi-libc library
    # set(WASI_LIBC "-L${WASI_LIBC_PATH}/lib -lwasi-libc -l libc-wasm -l wasm32-wasi")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -fPIC -target wasm32-wasi")
else()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")
    # set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# -Wall -Wextra)
endif()
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")
# set the project name and version
set (PROJECT_NAME "i2nsf_enarx")
project(${PROJECT_NAME} VERSION 1.0)




include(GNUInstallDirs)
include(CheckFunctionExists)
include(CheckIncludeFile)


# check the supported platform
if(NOT UNIX)
    message(FATAL_ERROR "Only *nix like systems are supported.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build-scripts/CMakeModules/")




if (${TARGET} STREQUAL "keystone")
    if(RISCV32)
    set(eyrie_plugins "freemem untrusted_io_syscall linux_syscall env_setup rv32")
    else()
    set(eyrie_plugins "freemem untrusted_io_syscall linux_syscall env_setup")
    endif()
endif()



set(SRC_DIRECTORY "${PROJECT_SOURCE_DIR}/src")

# Add files to the directory

# Include directories
include_directories("${SRC_DIRECTORY}")



# Only include this library for non trusted parts
if (${TARGET} STREQUAL "untrusted")
    message(STATUS "Including directories for untrusted target")
    include_directories("${SRC_DIRECTORY}/pfkeyv2_utils")
    link_directories("${SRC_DIRECTORY}/pfkeyv2_utils")
    include_directories("${SRC_DIRECTORY}/sysrepo_utils")
    link_directories("${SRC_DIRECTORY}/sysrepo_utils")
endif()



if (${SYSREPO})
    message(STATUS "Add libraries for untrusted")
    add_library(i2nsf STATIC
    "${SRC_DIRECTORY}/sad_entry/sad_entry.c"
    "${SRC_DIRECTORY}/sad_entry/sad_entry.c"
    "${SRC_DIRECTORY}/spd_entry/spd_entry.c"
    "${SRC_DIRECTORY}/utils/utils.c" 
    "${SRC_DIRECTORY}/utils/map.c"
    "${SRC_DIRECTORY}/messages/messages.c"
    "${SRC_DIRECTORY}/messages/trust_handler.c"
    "${SRC_DIRECTORY}/pfkeyv2_utils/pfkeyv2_utils.c"
    "${SRC_DIRECTORY}/pfkeyv2_utils/pfkeyv2_entry.c"
    "${SRC_DIRECTORY}/sysrepo_utils/sysrepo_utils.c"
    "${SRC_DIRECTORY}/sysrepo_utils/sysrepo_entries.c"
    "${SRC_DIRECTORY}/sysrepo_utils/sysrepo_print.c"
    "${SRC_DIRECTORY}/log/log.c"
)
else()

    add_library(i2nsf STATIC 
        "${SRC_DIRECTORY}/sad_entry.c"
        "${SRC_DIRECTORY}/sad_entry.h"
        "${SRC_DIRECTORY}/spd_entry.c"
        "${SRC_DIRECTORY}/spd_entry.h"
        "${SRC_DIRECTORY}/utils.c" 
        "${SRC_DIRECTORY}/utils.h" 
        "${SRC_DIRECTORY}/map.c"
        "${SRC_DIRECTORY}/map.h"
        "${SRC_DIRECTORY}/messages.c"
        "${SRC_DIRECTORY}/messages.h"
        "${SRC_DIRECTORY}/trust_handler.c"
        "${SRC_DIRECTORY}/trust_handler.h"
        "${SRC_DIRECTORY}/log.c"
        "${SRC_DIRECTORY}/log.h"
        "${PROJECT_SOURCE_DIR}/cmd/i2nsf_enarx/main.c"
    )
    set_target_properties(i2nsf PROPERTIES LINK_FLAGS "-nostdlib")
endif()

# Add parson
add_library(parson STATIC "${PROJECT_SOURCE_DIR}/parson/parson.c")
# set_target_properties(parson PROPERTIES PRIVATE_HEADER "parson.h")
include_directories("${PROJECT_SOURCE_DIR}/parson")
include_directories("${SRC_DIRECTORY}")
# # Add hashmap
# add_library(hashmap "${PROJECT_SOURCE_DIR}/c-hashmap/map.c")
# set_target_properties(hashmap PROPERTIES PRIVATE_HEADER "map.h")
# include_directories("${PROJECT_SOURCE_DIR}/c-hashmap")


# add the executable
if (${TARGET} STREQUAL "untrusted")
    add_executable(${PROJECT_NAME} 
    "${PROJECT_SOURCE_DIR}/cmd/i2nsf_ra/main.c"
    "${SRC_DIRECTORY}/constants.h"
    )
else()
    add_executable(${PROJECT_NAME} 
        "wrapper.c"
        "${SRC_DIRECTORY}/constants.h"
    )
endif()




 # Only include this for the unstrusted binary
if (USE_SYSREPO)   
    # Inlude sysrepo
    include("${CMAKE_SOURCE_DIR}/build-scripts/ImportSysrepo.cmake")
    message(STATUS "Linking librararies for untrusted target")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        i2nsf
        parson     
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        i2nsf
        parson     
    )
endif()

# Add Base to I2NSF target_include_directories()
# target_include_directories(${PROJECT_NAME} PRIVATE log "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/log")
# target_include_directories(${PROJECT_NAME} PRIVATE utils "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/utils" )
# target_include_directories(${PROJECT_NAME} PRIVATE sad_entry "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/sad_entry")
# target_include_directories(${PROJECT_NAME} PRIVATE spd_entry "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/spd_entry")               
# target_include_directories(${PROJECT_NAME} PRIVATE spd_entry "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/messages")      
# target_include_directories(${PROJECT_NAME} PRIVATE parson "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/parson")
# target_include_directories(${PROJECT_NAME} PRIVATE hashmap "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/c-hashmap")

# Only include this for the unstrusted binary
if (${TARGET} STREQUAL "untrusted")
    message(STATUS "Target incldue for untrusted")
    target_include_directories(${PROJECT_NAME} PRIVATE pfkeyv2_utils "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/pfkeyv2_utils")
    target_include_directories(${PROJECT_NAME} PRIVATE sysrepo_utils "${PROJECT_BINARY_DIR}" "${SRC_DIRECTORY}/sysrepo_utils")
endif()

# if (WASI_TARGET)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${WASI_LIBC})
# endif()

if (${TARGET} STREQUAL "untrusted")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "i2nsf_untrusted")
elseif(${TARGET} STREQUAL "enarx")
        # set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "i2nsf")
elseif(${TARGET} STREQUAL "keystone")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "i2nsf_keystone")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "i2nsf_unknown")
endif()



# TODO Add for the untrusted case
# We need to install yang modules
# We need to enable sudo "sysrepoctl -e ikeless-notification -a -c  ietf-i2nsf-ikeless -v3"

# add the binary tree to the search path for include files
target_include_directories(${PROJECT_NAME} PRIVATE
                            "${PROJECT_BINARY_DIR}"
                        )




if (${TARGET} STREQUAL "enarx")
    install(TARGETS i2nsf
            EXPORT i2nsf-config
            ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/libs
            LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/libs)
    install(TARGETS parson
            ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/libs
            LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/libs)
    install(FILES 
    "${SRC_DIRECTORY}/trust_handler.h"
    "${SRC_DIRECTORY}/sad_entry.h"
    "${SRC_DIRECTORY}/spd_entry.h"
    "${SRC_DIRECTORY}/utils.h" 
    "${SRC_DIRECTORY}/map.h"
    "${SRC_DIRECTORY}/messages.h"
    "${SRC_DIRECTORY}/log.h"
    "${PROJECT_SOURCE_DIR}/parson/parson.h"
    "${SRC_DIRECTORY}/constants.h"
    DESTINATION ${PROJECT_SOURCE_DIR}/libs)
endif()
