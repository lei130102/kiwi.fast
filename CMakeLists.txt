cmake_minimum_required(VERSION 3.9)

if(MSVC)
    set(VCPKG_CMAKE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    set(VCPKG_TARGET_TRIPLET "x64-windows")
    set(CMAKE_TOOLCHAIN_FILE ${VCPKG_CMAKE})
endif()

project(kiwi.fast)

if(MSVC)
    set(CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CMakeModules;$ENV{VCPKG_ROOT};${CMAKE_MODULE_PATH}")
    set(CMAKE_PREFIX_PATH "$ENV{QTVCPKG};$ENV{VCPKG_ROOT};${CMAKE_PREFIX_PATH}")
else()
    set(CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CMakeModules;${CMAKE_MODULE_PATH}")
    set(CMAKE_PREFIX_PATH "$ENV{QTVCPKG};${CMAKE_PREFIX_PATH}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Og -m64 -Wa,-mbig-obj")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

set(BUILD_RUNTIME_DIR "bin")
set(BUILD_LIBRARY_DIR "lib")
set(BUILD_ARCHIVE_DIR "lib")

#######

set(SOURCE_DIR ${${PROJECT_NAME}_SOURCE_DIR}/src)
set(BINARY_DIR ${${PROJECT_NAME}_BINARY_DIR}/src)

set(KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.plugin_utility)
set(KIWI.FAST.PLUGIN_UTILITY_BINARY_DIR ${BINARY_DIR}/kiwi.fast.plugin_utility)

set(KIWI.FAST.UTILITY_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.utility)
set(KIWI.FAST.UTILITY_BINARY_DIR ${BINARY_DIR}/kiwi.fast.utility)

set(KIWI.FAST.MODEL_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.model)
set(KIWI.FAST.MODEL_BINARY_DIR ${BINARY_DIR}/kiwi.fast.model)

set(KIWI.FAST.PLUGIN_MANAGER_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.plugin_manager)
set(KIWI.FAST.PLUGIN_MANAGER_BINARY_DIR ${BINARY_DIR}/kiwi.fast.plugin_manager)

set(KIWI.FAST.PLUGIN_LIB_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.plugin_lib)
set(KIWI.FAST.PLUGIN_LIB_BINARY_DIR ${BINARY_DIR}/kiwi.fast.plugin_lib)

set(INTERFACE_DIR ${SOURCE_DIR}/interface)

set(KIWI.FAST.SERVER_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.server)
set(KIWI.FAST.SERVER_BINARY_DIR ${BINARY_DIR}/kiwi.fast.server)

set(KIWI.FAST.BATCH_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.batch)
set(KIWI.FAST.BATCH_BINARY_DIR ${BINARY_DIR}/kiwi.fast.batch)

set(KIWI.FAST.CLIENT_SOURCE_DIR ${SOURCE_DIR}/kiwi.fast.client)
set(KIWI.FAST.CLIENT_BINARY_DIR ${BINARY_DIR}/kiwi.fast.client)

set(TEST_SOURCE_DIR ${SOURCE_DIR}/test)
set(TEST_BINARY_DIR ${BINARY_DIR}/test)

#####

find_package(Boost REQUIRED COMPONENTS locale thread log program_options filesystem context)

#find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport Xml OpenGL Concurrent REQUIRED)

find_package(SQLite3 REQUIRED)

######

include_directories(${SOURCE_DIR})

include_directories(${KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR})
include_directories(${KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR}/detail)

include_directories(${KIWI.FAST.UTILITY_SOURCE_DIR})
include_directories(${KIWI.FAST.UTILITY_SOURCE_DIR}/detail)

include_directories(${KIWI.FAST.PLUGIN_MANAGER_SOURCE_DIR})
include_directories(${KIWI.FAST.PLUGIN_MANAGER_SOURCE_DIR}/detail)

include_directories(${KIWI.FAST.PLUGIN_LIB_SOURCE_DIR})
include_directories(${KIWI.FAST.PLUGIN_LIB_SOURCE_DIR}/detail)

include_directories(${KIWI.FAST.MODEL_SOURCE_DIR})
include_directories(${KIWI.FAST.MODEL_SOURCE_DIR}/detail)

include_directories(${INTERFACE_DIR})

include_directories(${KIWI.FAST.SERVER_SOURCE_DIR})
include_directories(${KIWI.FAST.SERVER_SOURCE_DIR}/detail)

include_directories(${KIWI.FAST.BATCH_SOURCE_DIR})
include_directories(${KIWI.FAST.BATCH_SOURCE_DIR}/detail)

include_directories(${KIWI.FAST.CLIENT_SOURCE_DIR})
include_directories(${KIWI.FAST.CLIENT_SOURCE_DIR}/detail)

include_directories(${TEST_SOURCE_DIR})
include_directories(${TEST_SOURCE_DIR}/detail)

########

message(STATUS "CMAKE_TOOLCHAIN_FILE                    =${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "CMAKE_INSTALL_PREFIX                    =${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_MODULE_PATH                       =${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH                       =${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_BUILD_TYPE                        =${CMAKE_BUILD_TYPE}")
message(STATUS "----------------------------------------------------------------")
message(STATUS "SOURCE_DIR                              =${SOURCE_DIR}")
message(STATUS "BINARY_DIR                              =${BINARY_DIR}")
message(STATUS "KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR     =${KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR}")
message(STATUS "KIWI.FAST.PLUGIN_UTILITY_BINARY_DIR     =${KIWI.FAST.PLUGIN_UTILITY_BINARY_DIR}")
message(STATUS "KIWI.FAST.UTILITY_SOURCE_DIR            =${KIWI.FAST.UTILITY_SOURCE_DIR}")
message(STATUS "KIWI.FAST.UTILITY_BINARY_DIR            =${KIWI.FAST.UTILITY_BINARY_DIR}")
message(STATUS "KIWI.FAST.PLUGIN_MANAGER_SOURCE_DIR     =${KIWI.FAST.PLUGIN_MANAGER_SOURCE_DIR}")
message(STATUS "KIWI.FAST.PLUGIN_MANAGER_BINARY_DIR     =${KIWI.FAST.PLUGIN_MANAGER_BINARY_DIR}")
message(STATUS "KIWI.FAST.PLUGIN_LIB_SOURCE_DIR         =${KIWI.FAST.PLUGIN_LIB_SOURCE_DIR}")
message(STATUS "KIWI.FAST.PLUGIN_LIB_BINARY_DIR         =${KIWI.FAST.PLUGIN_LIB_BINARY_DIR}")
message(STATUS "KIWI.FAST.MODEL_SOURCE_DIR              =${KIWI.FAST.MODEL_SOURCE_DIR}")
message(STATUS "KIWI.FAST.MODEL_BINARY_DIR              =${KIWI.FAST.MODEL_BINARY_DIR}")
message(STATUS "INTERFACE_DIR                           =${INTERFACE_DIR}")
message(STATUS "KIWI.FAST.SERVER_SOURCE_DIR             =${KIWI.FAST.SERVER_SOURCE_DIR}")
message(STATUS "KIWI.FAST.SERVER_BINARY_DIR             =${KIWI.FAST.SERVER_BINARY_DIR}")
message(STATUS "KIWI.FAST.BATCH_SOURCE_DIR              =${KIWI.FAST.BATCH_SOURCE_DIR}")
message(STATUS "KIWI.FAST.BATCH_BINARY_DIR              =${KIWI.FAST.BATCH_BINARY_DIR}")
message(STATUS "KIWI.FAST.CLIENT_SOURCE_DIR             =${KIWI.FAST.CLIENT_SOURCE_DIR}")
message(STATUS "KIWI.FAST.CLIENT_BINARY_DIR             =${KIWI.FAST.CLIENT_BINARY_DIR}")
message(STATUS "TEST_SOURCE_DIR                         =${TEST_SOURCE_DIR}")
message(STATUS "TEST_BINARY_DIR                         =${TEST_BINARY_DIR}")

message(STATUS "Boost_CONTEXT_LIBRARY                   =${Boost_CONTEXT_LIBRARY}")
message(STATUS "Boost_LOCALE_LIBRARY                    =${Boost_LOCALE_LIBRARY}")
message(STATUS "Boost_THREAD_LIBRARY                    =${Boost_THREAD_LIBRARY}")
message(STATUS "Boost_LOG_SETUP_LIBRARY                 =${Boost_LOG_SETUP_LIBRARY}")
message(STATUS "Boost_LOG_LIBRARY                       =${Boost_LOG_LIBRARY}")
message(STATUS "Boost_PROGRAM_OPTIONS_LIBRARY           =${Boost_PROGRAM_OPTIONS_LIBRARY}")
message(STATUS "Boost_FILESYSTEM_LIBRARY                =${Boost_FILESYSTEM_LIBRARY}")

#####

add_subdirectory(${KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.UTILITY_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.MODEL_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.BATCH_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.SERVER_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.CLIENT_SOURCE_DIR})
#add_subdirectory(${TEST_SOURCE_DIR})
