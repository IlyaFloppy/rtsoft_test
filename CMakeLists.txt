cmake_minimum_required(VERSION 3.15)
project(rtsoft_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(rtsoft_test main.cpp file_server.cpp file_server.h)