cmake_minimum_required(VERSION 3.10)

# check the supported platform
if(NOT UNIX)
    message(FATAL_ERROR "Only *nix like systems are supported.")
endif()


set(SRC_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
# Include directories
include_directories("${SRC_DIRECTORY}")


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
        # "${PROJECT_SOURCE_DIR}/cmd/i2nsf_enarx/main.c"
    )
set_target_properties(i2nsf PROPERTIES LINK_FLAGS "-nostdlib")


# Add parson
add_library(parson STATIC "${PROJECT_SOURCE_DIR}/parson/parson.c")
include_directories("${PROJECT_SOURCE_DIR}/parson")
include_directories("${SRC_DIRECTORY}")

# add_executable(${PROJECT_NAME} 
#     "wrapper.c"
#     "${SRC_DIRECTORY}/constants.h"
# )

# target_link_libraries(${PROJECT_NAME} PRIVATE 
# i2nsf
# parson     
# )

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

