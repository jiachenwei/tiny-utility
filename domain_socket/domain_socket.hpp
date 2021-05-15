/**
 * @file domain_socket.hpp
 * @brief 声名了有关域套接字操作的一些函数
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-15
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../socket_message.hpp"

int init_tcp_domain_server(const char *const socket_addr);

int init_tcp_domain_client(const char *const socket_addr);

int recv_tcp_domain_msg(const int accpet_fd, const SocketMessage *msg);

int send_tcp_domain_msg(const int socket_fd, const SocketMessage *msg);

int close_tcp_domain_server(const int accpet_fd);

int close_tcp_domain_client(const int socket_fd);

///////////////////////////////////////////////////////////////////

int init_udp_domain_server(const char *const socket_addr);

int init_udp_domain_client(const char *const socket_addr);

int recv_udp_domain_msg(const int socket_fd, const SocketMessage *msg);

int send_udp_domain_msg(const int socket_fd, const SocketMessage *msg);

int close_udp_domain_server(const int socket_fd);

int close_udp_domain_client(const int socket_fd);

///////////////////////////////////////////////////////////////////

int close_all_tcp_domain_server();

int close_all_tcp_domain_client();

int close_all_udp_domain_server();

int close_all_udp_domain_client();
