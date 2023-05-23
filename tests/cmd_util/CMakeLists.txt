cmake_minimum_required(VERSION 3.15)

#test for cmd_util.cpp
add_executable(cmd_util main.cpp)
target_link_libraries(cmd_util
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    test_util
    )

add_test(
    NAME cmd_util
    COMMAND $<TARGET_FILE:cmd_util> "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
    )
