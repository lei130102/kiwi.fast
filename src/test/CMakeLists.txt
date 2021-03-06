function(add_test test_name test_file)
    add_executable(${test_name}
    ${test_file})

    target_include_directories(${test_name} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${test_name} PRIVATE kiwi.fast.utility kiwi.fast.plugin_utility ${Boost_LIBRARIES})

    if(MSYS64)
        target_compile_options(${test_name} PUBLIC -municode)
        target_link_options(${test_name} PUBLIC -municode)
    else()
        target_compile_options(${test_name} PUBLIC /source-charset:utf-8)
    endif()

    set_target_properties(${test_name} PROPERTIES OUTPUT_NAME "${test_name}$<$<CONFIG:Debug>:_d>")

    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
    set_target_properties(${test_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
    set_target_properties(${test_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
    set_target_properties(${test_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")
    set_target_properties(${test_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")

endfunction(add_test)

add_test(test_example
    "${CMAKE_CURRENT_SOURCE_DIR}/test_example.cpp")

add_test(test_command_line
    "${CMAKE_CURRENT_SOURCE_DIR}/test_command_line.cpp")

add_test(test_setting
    "${CMAKE_CURRENT_SOURCE_DIR}/test_setting.cpp")

add_test(test_test
    "${CMAKE_CURRENT_SOURCE_DIR}/test_test.cpp")

#add_test(test_code_convert
#    "${CMAKE_CURRENT_SOURCE_DIR}/test_code_convert.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/test_code_convert.h"
#    )

#add_test(test_exception
#    "${CMAKE_CURRENT_SOURCE_DIR}/test_exception.cpp")
