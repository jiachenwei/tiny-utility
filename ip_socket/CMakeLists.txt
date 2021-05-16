cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILE ip_socket.cpp ip_socket.hpp)
add_library(ip_socket ${SOURCE_FILE})

