file(GLOB_RECURSE SRC_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SRC_HDR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB INTERFACE_HDR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE DETAIL_HDR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/detail/*.h")

set(SRC_LIST ${SRC_SRC_LIST} ${SRC_HDR_LIST})
set(HDR_LIST ${DETAIL_HDR_LIST} ${INTERFACE_HDR_LIST})

############################
message(STATUS "-----------------------------------------")
message(STATUS "SRC_LIST                    =${SRC_LIST}")
message(STATUS "HDR_LIST                    =${HDR_LIST}")

add_executable(kiwi.fast.server
    ${SRC_LIST}
    ${HDR_LIST}
    )

target_include_directories(kiwi.fast.server PRIVATE ${Boost_INCLUDE_DIRS})

#kiwi.plugin_utility依赖其他库的信息也会被设置到kiwi上
target_link_libraries(kiwi.fast.server
    PUBLIC SQLite3 ${Boost_CONTEXT_LIBRARY} ${Boost_LOCALE_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${Boost_LOG_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}
    PRIVATE kiwi.fast.model kiwi.fast.utility kiwi.fast.plugin_utility wsock32 ws2_32)

if(MSVC)
    target_compile_options(kiwi.fast.server PUBLIC /source-charset:utf-8)
else()
    target_compile_options(kiwi.fast.server PUBLIC -municode)
    target_link_options(kiwi.fast.server PUBLIC -municode)
endif()

set_target_properties(kiwi.fast.server PROPERTIES OUTPUT_NAME "kiwi.fast.server$<$<CONFIG:Debug>:_d>")
set_target_properties(kiwi.fast.server PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
set_target_properties(kiwi.fast.server PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
set_target_properties(kiwi.fast.server PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
set_target_properties(kiwi.fast.server PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
set_target_properties(kiwi.fast.server PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")
set_target_properties(kiwi.fast.server PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")
