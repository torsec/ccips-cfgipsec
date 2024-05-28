cmake_minimum_required(VERSION 3.10)

# set(host_bin keystone-ccips-runner)

# Include other stuff related with sysrepo
find_package(Threads REQUIRED)  
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_THREAD_PREFER_PTHREAD ON) 
target_link_libraries(${APP_NAME} PUBLIC pthread)


message(STATUS "Looking for libyang")
find_package(LibYANG REQUIRED)
target_link_libraries(${APP_NAME} PUBLIC ${LIBYANG_LIBRARIES})
include_directories(${LIBYANG_INCLUDE_DIRS})
list(APPEND CMAKE_REQUIRED_INCLUDES ${LIBYANG_INCLUDE_DIRS})
list(APPEND CMAKE_REQUIRED_LIBRARIES ${LIBYANG_LIBRARIES})


message(STATUS "Looking for Sysrepo")
find_package(Sysrepo REQUIRED)
target_link_libraries(${APP_NAME} PUBLIC ${SYSREPO_LIBRARIES})
include_directories(${SYSREPO_INCLUDE_DIRS})
list(APPEND CMAKE_REQUIRED_INCLUDES ${SYSREPO_INCLUDE_DIRS})
list(APPEND CMAKE_REQUIRED_LIBRARIES ${SYSREPO_LIBRARIES})

# message(STATUS "Looking for Python")
# find_package(PythonLibs REQUIRED)
# target_link_libraries(${APP_NAME} PUBLIC ${PYTHON_LIBRARIES})
# include_directories(${PYTHON_INCLUDE_DIRS})
# list(APPEND CMAKE_REQUIRED_INCLUDES ${PYTHON_INCLUDE_DIRS})
# list(APPEND CMAKE_REQUIRED_LIBRARIES ${PYTHON_LIBRARIES})
