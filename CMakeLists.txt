cmake_minimum_required(VERSION 2.8)

set(MSYS64 OFF)

if(MSYS64)
    set(LIB_DIR "D:/msys64/mingw64")
else()
    #set(LIB_DIR "D:/OtherDocuments/vcpkg/vcpkg/installed/x64-windows")
    set(LIB_DIR "C:/vcpkg/installed/x64-windows")
    set(VCPKG_CMAKE "${LIB_DIR}/../../scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    set(VCPKG_TARGET_TRIPLET "x64-windows")
    set(CMAKE_TOOLCHAIN_FILE ${VCPKG_CMAKE})
endif()

project(kiwi.fast)

set(CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CMakeModules;${CMAKE_MODULE_PATH}")
if(MSYS64)
else()
    set(CMAKE_PREFIX_PATH "$ENV{QTVCPKG};$ENV{VCPKG_ROOT};${CMAKE_PREFIX_PATH}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSYS64)
#g启用调试信息 O优化级别 注意这两个参数需要跟后面配合使用，可以解决大obj编译失败问题
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Og -m64 -Wa,-mbig-obj")
else()
#/EHa          vc编译器在release版默认是同步异常，做了优化，只会检查代码中有显式throw的异常，不能捕获非法内存访问、除零等结构化异常，修改release的编译选项，加个/EHa就好了
#/Od           禁止优化
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa /Od /bigobj")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(MSYS64)
else()
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

set(BOOST_INCLUDEDIR ${LIB_DIR}/include)
set(BOOST_LIBRARYDIR ${LIB_DIR}/lib)

message(STATUS "BOOST_ROOT                              =${BOOST_ROOT}")
message(STATUS "BOOST_INCLUDEDIR                        =${BOOST_INCLUDEDIR}")
message(STATUS "BOOST_LIBRARYDIR                        =${BOOST_LIBRARYDIR}")

find_package(Boost REQUIRED COMPONENTS locale thread log program_options filesystem context)

message(STATUS "Boost_INCLUDE_DIRS                      =${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES                         =${Boost_LIBRARIES}")
message(STATUS "Boost_CONTEXT_LIBRARY                   =${Boost_CONTEXT_LIBRARY}")
message(STATUS "Boost_LOCALE_LIBRARY                    =${Boost_LOCALE_LIBRARY}")
message(STATUS "Boost_THREAD_LIBRARY                    =${Boost_THREAD_LIBRARY}")
message(STATUS "Boost_LOG_SETUP_LIBRARY                 =${Boost_LOG_SETUP_LIBRARY}")
message(STATUS "Boost_LOG_LIBRARY                       =${Boost_LOG_LIBRARY}")
message(STATUS "Boost_PROGRAM_OPTIONS_LIBRARY           =${Boost_PROGRAM_OPTIONS_LIBRARY}")
message(STATUS "Boost_FILESYSTEM_LIBRARY                =${Boost_FILESYSTEM_LIBRARY}")

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


#####

add_subdirectory(${KIWI.FAST.PLUGIN_UTILITY_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.UTILITY_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.MODEL_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.BATCH_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.SERVER_SOURCE_DIR})
add_subdirectory(${KIWI.FAST.CLIENT_SOURCE_DIR})
add_subdirectory(${TEST_SOURCE_DIR})
