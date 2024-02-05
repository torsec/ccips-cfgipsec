include(${KEYSTONE_SDK_DIR}/../macros.cmake)

# eapp
use_riscv_musl_toolchain(64)

set(eapp_src eapp.c)

add_executable(${eapp_bin} ${eapp_src})

target_include_directories(${eapp_bin}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/app
  PUBLIC ${KEYSTONE_SDK_DIR}/include/edge
  PUBLIC ${PPOJECT_SOURCE_DIR}/libs)

target_link_libraries(${eapp_bin} "-static -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds" ${KEYSTONE_LIB_EAPP} ${KEYSTONE_LIB_EDGE} ${LIB_I2NSF} ${LIB_PARSON})
