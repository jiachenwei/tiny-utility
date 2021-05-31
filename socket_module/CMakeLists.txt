cmake_minimum_required(VERSION 3.10)

# set the project name
project(domain_socket_test VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory( domain_socket )
add_subdirectory( ip_socket )

include_directories( ./domain_socket ./ip_socket)

link_directories( ./domain_socket ./ip_socket)

set(domain_socket_test_server_source demo/domain_socket_test_server.cpp socket_message.hpp)
set(domain_socket_test_client_source demo/domain_socket_test_client.cpp socket_message.hpp)
set(ip_socket_test_server_source demo/ip_socket_test_server.cpp socket_message.hpp)
set(ip_socket_test_client_source demo/ip_socket_test_client.cpp socket_message.hpp)

add_executable(domain_socket_test_server ${domain_socket_test_server_source})
add_executable(domain_socket_test_client ${domain_socket_test_client_source})
add_executable(ip_socket_test_server ${ip_socket_test_server_source})
add_executable(ip_socket_test_client ${ip_socket_test_client_source})

target_link_libraries(domain_socket_test_server domain_socket)
target_link_libraries(domain_socket_test_client domain_socket)
target_link_libraries(ip_socket_test_server ip_socket)
target_link_libraries(ip_socket_test_client ip_socket)