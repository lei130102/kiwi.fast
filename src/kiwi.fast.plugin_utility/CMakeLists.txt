file(GLOB_RECURSE SRC_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SRC_HDR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB         INTERFACE_HDR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE DETAIL_HDR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/detail/*.h")

set(SRC_LIST ${SRC_SRC_LIST} ${SRC_HDR_LIST})
set(HDR_LIST ${DETAIL_HDR_LIST} ${INTERFACE_HDR_LIST})

##################################

message(STATUS "----------------------------------------")
message(STATUS "SRC_LIST                    =${SRC_LIST}")
message(STATUS "HDR_LIST                    =${HDR_LIST}")

add_library(kiwi.fast.plugin_utility
    STATIC
    ${HDR_LIST}
    ${SRC_LIST})

if(MSYS64)
    target_compile_options(kiwi.fast.plugin_utility PUBLIC -municode)
    target_link_options(kiwi.fast.plugin_utility PUBLIC -municode)
else()
    target_compile_options(kiwi.fast.plugin_utility PUBLIC /source-charset:utf-8)
endif()

target_include_directories(kiwi.fast.plugin_utility PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(kiwi.fast.plugin_utility PUBLIC ${Boost_LIBRARIES})

set_target_properties(kiwi.fast.plugin_utility PROPERTIES OUTPUT_NAME "kiwi.fast.plugin_utility$<$<CONFIG:Debug>:_d>")

set_target_properties(kiwi.fast.plugin_utility PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
set_target_properties(kiwi.fast.plugin_utility PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
set_target_properties(kiwi.fast.plugin_utility PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
set_target_properties(kiwi.fast.plugin_utility PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
set_target_properties(kiwi.fast.plugin_utility PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")
set_target_properties(kiwi.fast.plugin_utility PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")
