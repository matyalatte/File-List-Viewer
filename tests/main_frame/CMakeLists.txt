cmake_minimum_required(VERSION 3.15)

#test for main_frame.cpp
add_executable(main_frame main.cpp)
target_link_libraries(main_frame
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    test_util
    )

add_test(
    NAME main_frame
    COMMAND $<TARGET_FILE:main_frame> "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
    )
