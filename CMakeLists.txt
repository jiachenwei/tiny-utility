cmake_minimum_required(VERSION 3.10)

# set the project name
project(domain_socket_test VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory( domain_socket )

include_directories( ./domain_socket )

link_directories( ./domain_socket )

set(domain_socket_test_server_source domain_socket_test_server.cpp socket_message.hpp)
set(domain_socket_test_client_source domain_socket_test_client.cpp socket_message.hpp)

add_executable(domain_socket_test_server ${domain_socket_test_server_source})
add_executable(domain_socket_test_client ${domain_socket_test_client_source})

target_link_libraries(domain_socket_test_server domain_socket)
target_link_libraries(domain_socket_test_client domain_socket)