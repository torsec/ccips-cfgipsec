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

use_riscv_toolchain(${BITS})

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
target_link_libraries(${host_bin} ${KEYSTONE_LIB_HOST} ${KEYSTONE_LIB_EDGE})

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
