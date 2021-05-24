/**
 * @file ip_socket.hpp
 * @brief 声名了有关ip套接字操作的一些函数
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-16
 */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

#include "../socket_message.hpp"

#define MAX_LISTEN_NUM 10

int init_tcp_ip_server(const uint port);
int init_tcp_ip_client(const char* const ip_addr, const uint port);
int recv_tcp_ip_msg(const int socket_fd, const SocketMessage* msg);
int recv_tcp_ip_msg_durable(const int& socket_fd, int& accept_fd,
                            const SocketMessage* msg);
int send_tcp_ip_msg(const int socket_fd, const SocketMessage* msg);
int close_tcp_ip_server(const int socket_fd);
int close_tcp_ip_client(const int socket_fd);

int init_udp_ip_server(const uint port);
int init_udp_ip_client(const char* const ip_addr, const uint port);
int recv_udp_ip_msg(const int socket_fd, const SocketMessage* msg);
int send_udp_ip_msg(const int socket_fd, const SocketMessage* msg);
int close_udp_ip_server(const int socket_fd);
int close_udp_ip_client(const int socket_fd);

int close_all_tcp_ip_server();
int close_all_tcp_ip_client();
int close_all_udp_ip_server();
int close_all_udp_ip_client();