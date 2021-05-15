/**
 * @file ip_socket.cpp
 * @brief
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-16
 */

#include "ip_socket.hpp"

#include <iostream>
#include <vector>

typedef struct SocketFileDescriptor {
    int socket_fd;
    int accept_fd;
} SocketFileDescriptor;

std::vector<SocketFileDescriptor> tcp_ip_socket_server_list;
std::vector<int> udp_ip_socket_server_list;

std::vector<int> tcp_ip_socket_client_list;
std::vector<int> udp_ip_socket_client_list;

int init_tcp_ip_server(const uint port) {
    int ret = 0;
    struct sockaddr_in server_addr;
    SocketFileDescriptor fd;

    // 1. 创建套接字
    std::cout << "Socket create...";

    fd.socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd.socket_fd < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 2. 绑定套接字
    std::cout << "Binding socket...";
    ret = bind(fd.socket_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 3. 监听套接字
    std::cout << "Listen socket...";
    ret = listen(fd.socket_fd, 10);  // 最大监听数量10

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    std::cout << "Waiting for new requests...";
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    ret = fd.accept_fd =
        accept(fd.socket_fd, (struct sockaddr*)&client_addr, &length);

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    tcp_ip_socket_server_list.push_back(fd);

    return fd.accept_fd;
}

int init_tcp_ip_client(const char* const ip_addr, const uint port) {
    ;
    ;
}

int recv_tcp_ip_msg(const int accpet_fd, const SocketMessage* msg) {
    ;
    ;
}

int send_tcp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    ;
    ;
}

int close_tcp_ip_server(const int accpet_fd) {
    ;
    ;
}

int close_tcp_ip_client(const int socket_fd) {
    ;
    ;
}

int init_udp_ip_server(const uint port) {
    ;
    ;
}

int init_udp_ip_client(const char* const ip_addr, const uint port) {
    ;
    ;
}

int recv_udp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    ;
    ;
}

int send_udp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    ;
    ;
}

int close_udp_ip_server(const int socket_fd) {
    ;
    ;
}

int close_udp_ip_client(const int socket_fd) {
    ;
    ;
}

int close_all_tcp_ip_server() {
    ;
    ;
}

int close_all_tcp_ip_client() {
    ;
    ;
}

int close_all_udp_ip_server() {
    ;
    ;
}

int close_all_udp_ip_client() {
    ;
    ;
}