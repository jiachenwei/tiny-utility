cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILE domain_socket.cpp domain_socket.hpp)
add_library(domain_socket ${SOURCE_FILE})

