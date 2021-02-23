function(add_test test_name test_file)
    add_executable(${test_name}
    ${test_file}
    )

    target_include_directories(${test_name} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${test_name} PRIVATE kiwi.fast.plugin_utility ${Boost_LIBRARIES})

    if(MSVC)
        target_compile_options(kiwi.fast.plugin_utility PUBLIC /source-charset:utf-8)
    else()
        target_compile_options(kiwi.fast.plugin_utility PUBLIC -municode)
        target_link_options(kiwi.fast.plugin_utility PUBLIC -municode)
    endif()

    set_target_properties(${test_name} PROPERTIES OUTPUT_NAME "${test_name}$<$<CONFIG:Debug>:_d>")

    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME_DIR}")
    set_target_properties(${test_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
    set_target_properties(${test_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_LIBRARY_DIR}")
    set_target_properties(${test_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")
    set_target_properties(${test_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${BUILD_ARCHIVE_DIR}")

endfunction(add_test)

add_test(test_test
    "${CMAKE_CURRENT_SOURCE_DIR}/test_test.cpp")

add_test(test_code_convert
    "${CMAKE_CURRENT_SOURCE_DIR}/test_code_convert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_code_convert.h"
    )

add_test(test_exception
    "${CMAKE_CURRENT_SOURCE_DIR}/test_exception.cpp")
