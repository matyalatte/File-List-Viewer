cmake_minimum_required(VERSION 3.10)

include_directories(test_util)
add_library(test_util test_util/test_util.cpp)

#make test exe files
add_subdirectory(cmd_util)
add_subdirectory(main_frame)
