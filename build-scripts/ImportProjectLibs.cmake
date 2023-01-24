cmake_minimum_required(VERSION 3.10)

# Add general libraries
add_library(log "../${SRC_DIRECTORY}/log/log.c")
add_library(sad_entry "../${SRC_DIRECTORY}/sad_entry/sad_entry.c")
add_library(spd_entry "../${SRC_DIRECTORY}/spd_entry/spd_entry.c")
add_library(utils "../${SRC_DIRECTORY}/utils/utils.c")

# Add parson
add_library(parson "${PROJECT_SOURCE_DIR}/parson/parson.c")
set_target_properties(parson PROPERTIES PUBLIC_HEADER "parson.h")
include_directories("${PROJECT_SOURCE_DIR}/parson")


# Include directories
include_directories("../${SRC_DIRECTORY}")
include_directories("../${SRC_DIRECTORY}/log")
include_directories("../${SRC_DIRECTORY}/utils")
include_directories("../${SRC_DIRECTORY}/sad_entry")
include_directories("../${SRC_DIRECTORY}/spd_entry")


link_directories("../${SRC_DIRECTORY}")
link_directories("../${SRC_DIRECTORY}/log")
link_directories("../${SRC_DIRECTORY}/utils")
link_directories("../${SRC_DIRECTORY}/sad_entry")
link_directories("../${SRC_DIRECTORY}/spd_entry")

SET(DEFAULT_LIBS log sad_entry spd_entry utils parson)


# Chec if we need sysrepo stuff
if (SYSREPO) 
    message(STATUS "Adding sysrepo handler libraries")
    add_library(pfkeyv2_utils 
    "../${SRC_DIRECTORY}/pfkeyv2_utils/pfkeyv2_utils.c"
    "../${SRC_DIRECTORY}/pfkeyv2_utils/pfkeyv2_entry.c"
    )
    add_library(sysrepo_utils 
    "../${SRC_DIRECTORY}/sysrepo_utils/sysrepo_utils.c"
    "../${SRC_DIRECTORY}/sysrepo_utils/sysrepo_entries.c"
    "../${SRC_DIRECTORY}/sysrepo_utils/sysrepo_print.c")
    message(STATUS "Including sysrepo libraries")
    include_directories("../${SRC_DIRECTORY}/pfkeyv2_utils")
    link_directories("../${SRC_DIRECTORY}/pfkeyv2_utils")
    include_directories("../${SRC_DIRECTORY}/sysrepo_utils")
    link_directories("../${SRC_DIRECTORY}/sysrepo_utils")
    # Add more libs
    SET(DEFAULT_LIBS sysrepo_utils pfkeyv2_utils log sad_entry spd_entry utils parson)
endif()
# Add parson
# target_link_libraries(${PROJECT_NAME} PUBLIC parson)




