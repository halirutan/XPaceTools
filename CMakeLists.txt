cmake_minimum_required(VERSION 3.15)
project(XPaceTools)

set(CMAKE_CXX_STANDARD 14)

add_executable(XPaceTools main.cpp xpace_log_file.cpp quaternion.cpp quaternion.h vector.cpp vector.h)
