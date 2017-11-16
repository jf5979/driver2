cmake_minimum_required(VERSION 3.6)
project(driver2)

set(CMAKE_CXX_STANDARD 11)
set(HEADER_FILES ./src/testbench.h)
set(SOURCE_FILES ./src/testbench.cpp)
add_executable(LAB2 ${SOURCE_FILES})