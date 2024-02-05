enable_language(C CXX ASM)

# check if SDK has been built and KEYSTONE_SDK_DIR environment variable exists
if (NOT DEFINED ENV{KEYSTONE_SDK_DIR})
  message(FATAL_ERROR "Please set KEYSTONE_SDK_DIR environment variable to an installed SDK path")
endif()
set(KEYSTONE_SDK_DIR $ENV{KEYSTONE_SDK_DIR})

include($ENV{KEYSTONE_SDK_DIR}/../macros.cmake)
include(ExternalProject)
find_package(Git REQUIRED)

if(RISCV32)
  message(STATUS "riscv32")
  set(BITS 32)
else()
  message(STATUS "riscv64")
  set(BITS 64)
endif()

if(KEYSTONE_LIB)
use_riscv_musl_toolchain(${BITS})
else()
use_riscv_toolchain(${BITS})
endif()

set(SRC_DIR cmd/i2nsf_keystone)

# export include directory
include_directories(${KEYSTONE_SDK_DIR}/include)

# set paths to the libraries
set(KEYSTONE_LIB_HOST ${KEYSTONE_SDK_DIR}/lib/libkeystone-host.a)
set(KEYSTONE_LIB_EDGE ${KEYSTONE_SDK_DIR}/lib/libkeystone-edge.a)
set(KEYSTONE_LIB_VERIFIER ${KEYSTONE_SDK_DIR}/lib/libkeystone-verifier.a)
set(KEYSTONE_LIB_EAPP ${KEYSTONE_SDK_DIR}/lib/libkeystone-eapp.a)

# find program "makeself"
find_program(MAKESELF makeself)

# check the supported platform
if(NOT UNIX)
    message(FATAL_ERROR "Only *nix like systems are supported.")
endif()

set(LIB_SRC_DIR "${PROJECT_SOURCE_DIR}/src")
# Include directories
include_directories("${LIB_SRC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/parson")

if(KEYSTONE_LIB)

add_library(i2nsf STATIC 
        "${LIB_SRC_DIR}/sad_entry.c"
        "${LIB_SRC_DIR}/sad_entry.h"
        "${LIB_SRC_DIR}/spd_entry.c"
        "${LIB_SRC_DIR}/spd_entry.h"
        "${LIB_SRC_DIR}/utils.c" 
        "${LIB_SRC_DIR}/utils.h" 
        "${LIB_SRC_DIR}/messages.c"
        "${LIB_SRC_DIR}/messages.h"
        "${LIB_SRC_DIR}/trust_handler.c"
        "${LIB_SRC_DIR}/trust_handler.h"
        "${LIB_SRC_DIR}/log.c"
        "${LIB_SRC_DIR}/log.h"
    )
set_target_properties(i2nsf PROPERTIES LINK_FLAGS "-static")
# target_include_directories(i2nsf
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/app
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/edge
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/host
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/verifier)
# target_link_libraries(i2nsf ${KEYSTONE_LIB_EAPP} ${KEYSTONE_LIB_EDGE} ${KEYSTONE_LIB_HOST} ${KEYSTONE_LIB_VERIFIER})

# Add parson
add_library(parson STATIC "${PROJECT_SOURCE_DIR}/parson/parson.c")
set_target_properties(parson PROPERTIES LINK_FLAGS "-static")
# target_include_directories(parson
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/app
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/edge
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/host
#   PUBLIC ${KEYSTONE_SDK_DIR}/include/verifier)
# target_link_libraries(parson ${KEYSTONE_LIB_EAPP} ${KEYSTONE_LIB_EDGE} ${KEYSTONE_LIB_HOST} ${KEYSTONE_LIB_VERIFIER})

install(TARGETS i2nsf
        EXPORT i2nsf-config
        ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/libs
        LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/libs)
install(TARGETS parson
        ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/libs
        LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/libs)
install(FILES 
"${LIB_SRC_DIR}/trust_handler.h"
"${LIB_SRC_DIR}/sad_entry.h"
"${LIB_SRC_DIR}/spd_entry.h"
"${LIB_SRC_DIR}/utils.h" 
"${LIB_SRC_DIR}/messages.h"
"${LIB_SRC_DIR}/log.h"
"${PROJECT_SOURCE_DIR}/parson/parson.h"
"${LIB_SRC_DIR}/constants.h"
DESTINATION ${PROJECT_SOURCE_DIR}/libs)

else()

set(LIB_I2NSF ${PROJECT_SOURCE_DIR}/libs/libi2nsf.a)
set(LIB_PARSON ${PROJECT_SOURCE_DIR}/libs/libparson.a)

# set application values
set(eapp_bin keystone-ccips)
set(eapp_src ${SRC_DIR}/eapp/eapp.c)
set(host_bin keystone-ccips-runner)
set(host_src ${SRC_DIR}/host/host.cpp)
set(package_name "keystone-ccips.ke")
set(package_script "./keystone-ccips-runner keystone-ccips eyrie-rt loader.bin")

if(RISCV32)
  set(eyrie_plugins "freemem io_syscall linux_syscall net_syscall env_setup rv32")
else()
  set(eyrie_plugins "freemem io_syscall linux_syscall net_syscall env_setup")
endif()

# eapp

add_subdirectory(${SRC_DIR}/eapp)

# host

add_executable(${host_bin} ${host_src})
# add -std=c++11 flag
# set_target_properties(${host_bin}
#   PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
# )
target_include_directories(${host_bin}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/host
  PUBLIC ${KEYSTONE_SDK_DIR}/include/edge
  PUBLIC ${PPOJECT_SOURCE_DIR}/libs)

target_link_libraries(${host_bin} ${KEYSTONE_LIB_HOST} ${KEYSTONE_LIB_EDGE} ${LIB_I2NSF} ${LIB_PARSON})

# add target for Eyrie runtime (see keystone.cmake)

set(eyrie_files_to_copy .options_log eyrie-rt)
add_eyrie_runtime(${eapp_bin}-eyrie
  ${eyrie_plugins}
  ${eyrie_files_to_copy})

# add target for loader

set(loader_files_to_copy loader.bin)
add_loader(${eapp_bin}-loader
  "1ed2b55fa0bf70067e7ad578affc5b49badbdb20"
  ${loader_files_to_copy})
  
# add target for packaging (see keystone.cmake)

add_keystone_package(${eapp_bin}-package
  ${package_name}
  ${package_script}
  ${eyrie_files_to_copy} ${SRC_DIR}/eapp/${eapp_bin} ${host_bin} ${loader_files_to_copy})

add_dependencies(${eapp_bin}-package ${eapp_bin}-eyrie ${eapp_bin}-loader)

endif()
