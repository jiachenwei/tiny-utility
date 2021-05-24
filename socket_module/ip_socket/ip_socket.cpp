/**
 * @file ip_socket.cpp
 * @brief
 * 实现了TCP与UDP下的ip套接字服务端与客户端，对于用户创建的多个ip套接字客户端与服务端可以实现自动内存管理。
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-16
 */

#include "ip_socket.hpp"

#include <iostream>
#include <vector>

std::vector<int> tcp_ip_socket_server_list;
std::vector<int> udp_ip_socket_server_list;

std::vector<int> tcp_ip_socket_client_list;
std::vector<int> udp_ip_socket_client_list;

/**
 * @brief 初始化一个tcp套接字服务端
 * @param  port             监听端口
 * @return int ip套接字服务端的socket_fd
 */
int init_tcp_ip_server(const uint port) {
    int ret = 0;
    int socket_fd = 0;
    struct sockaddr_in server_addr;

    // 1. 创建套接字
    std::cout << "Socket create...";

    ret = socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (ret < 0) {
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
    ret = bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 3. 监听套接字
    std::cout << "Listen socket...";
    ret = listen(socket_fd, 10);  // 最大监听数量10

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    tcp_ip_socket_server_list.push_back(socket_fd);

    return socket_fd;
}

/**
 * @brief 初始化一个tcp套接字客户端
 * @param  socket_addr      套接字地址
 * @param  port             发往服务端的端口
 * @return int 套接字客户端的socket_fd
 */
int init_tcp_ip_client(const char* const ip_addr, const uint port) {
    int ret = 0;
    int socket_fd = 0;
    struct sockaddr_in server_addr;
    ret = socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket create...";
    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    std::cout << "Connect socket...";
    ret =
        connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    return socket_fd;
}

/**
 * @brief tcp套接字服务端接收数据，服务端接收完数据主动释放连接
 * @param  socket_fd        服务端的socket_fd
 * @param  msg              数据缓存的指针
 * @return int 如果接收成功，返回接收的字节数;如果接收失败，返回-1
 */
int recv_tcp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    int ret = 0;
    int accept_fd = 0;

    std::cout << "Waiting for new requests...";

    ret = accept_fd = accept(socket_fd, NULL, NULL);

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    bzero(msg->buf, msg->len);
    ret = recv(accept_fd, msg->buf, msg->len, 0);
    close(accept_fd);

    return ret;
}

/**
 * @brief tcp套接字服务端接收数据，不主动释放链接
 * @param  socket_fd        服务端的socket_fd
 * @param  accept_fd
 * 服务端的accept_fd;如果为0,则建立新的accept_fd;如果不为零，则在此accept_fd上接收数据
 * @param  msg              存放数据的缓存指针
 * @return int
 * 如果接收成功，返回接收的字节数;如果接收失败，关闭accept_fd，返回-1
 */
int recv_tcp_ip_msg_durable(const int& socket_fd, int& accept_fd,
                            const SocketMessage* msg) {
    int ret = 0;
    if (accept_fd == 0) {
        std::cout << "Waiting for new requests...";
        ret = accept_fd = accept(socket_fd, NULL, NULL);
    }

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    bzero(msg->buf, msg->len);
    ret = recv(accept_fd, msg->buf, msg->len, 0);
    if (ret <= 0) {
        close(accept_fd);
        accept_fd = 0;
        std::cout << "request has been released!" << std::endl;
    }
    return ret;
}

/**
 * @brief tcp套接字客户端发送数据
 * @param  socket_fd        客户端的socket_fd
 * @param  msg              需要发送数据的指针
 * @return int 如果发送成功，返回发送的字节数;如果发送失败，返回-1
 */
int send_tcp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    return send(socket_fd, msg->buf, msg->len, 0);
}

/**
 * @brief 关闭一个tcp套接字服务端
 * @param  socket_fd        被关闭tcp套接服务端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_tcp_ip_server(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = tcp_ip_socket_server_list.begin();
         it != tcp_ip_socket_server_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > tcp_ip_socket_server_list.size()) return -1;
    close(socket_fd);

    tcp_ip_socket_server_list.erase(it);

    return 1;
}

/**
 * @brief 关闭一个tcp套接字客户端
 * @param  socket_fd        被关闭tcp套接字客户端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_tcp_ip_client(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = tcp_ip_socket_client_list.begin();
         it != tcp_ip_socket_client_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > tcp_ip_socket_client_list.size()) return -1;
    close(socket_fd);

    tcp_ip_socket_client_list.erase(it);

    return 1;
}

/**
 * @brief 初始化一个udp套接字服务端
 * @param  port             服务端监听端口
 * @return int udp套接字服务端的socket_fd
 */

int init_udp_ip_server(const uint port) {
    int ret = 0;
    struct sockaddr_in server_addr;
    int socket_fd = 0;

    // 1. 创建套接字
    std::cout << "Socket create...";

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
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
    ret = bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    udp_ip_socket_server_list.push_back(socket_fd);

    return socket_fd;
}

/**
 * @brief 初始化一个udp套接字客户端
 * @param  socket_addr      套接字地址
 * @param  port             发往服务端的端口
 * @return int udp套接字客户端的socket_fd
 */
int init_udp_ip_client(const char* const ip_addr, const uint port) {
    int socket_fd;
    int ret = 0;
    struct sockaddr_in server_addr;

    // 1. 创建套接字
    ret = socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    std::cout << "Socket create...";
    if (ret == -1) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    // 2. 连接套接字
    std::cout << "Connect socket...";
    ret = connect(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    if (ret == -1) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    udp_ip_socket_client_list.push_back(socket_fd);

    return socket_fd;
}

/**
 * @brief udp套接字服务端接收数据
 * @param  socket_fd        udp套接字服务端的socket_fd
 * @param  msg              数据缓存的指针
 * @return int 如果接收成功，返回接收的字节数;如果接收失败，返回-1
 */
int recv_udp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    bzero(msg->buf, msg->len);
    return recvfrom(socket_fd, msg->buf, msg->len, 0, NULL, NULL);
}

/**
 * @brief udp套接字客户端发送数据
 * @param  socket_fd        udp套接字客户端的socket_fd
 * @param  msg              数据缓存的指针
 * @return int 如果发送成功，返回发送的字节数;如果发送失败，返回-1
 */
int send_udp_ip_msg(const int socket_fd, const SocketMessage* msg) {
    return sendto(socket_fd, msg->buf, msg->len, 0, NULL, NULL);
}

/**
 * @brief 关闭一个udp套接字的服务端
 * @param  socket_fd        被关闭的udp套接字服务端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_udp_ip_server(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = udp_ip_socket_server_list.begin();
         it != udp_ip_socket_server_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > udp_ip_socket_server_list.size()) return -1;
    close(socket_fd);

    udp_ip_socket_server_list.erase(it);

    return 1;
}

/**
 * @brief 关闭一个udp套接字的客户端
 * @param  socket_fd        被关闭的udp套接字客户端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_udp_ip_client(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = udp_ip_socket_client_list.begin();
         it != udp_ip_socket_client_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > udp_ip_socket_client_list.size()) return -1;
    close(socket_fd);

    udp_ip_socket_client_list.erase(it);

    return 1;
}

/**
 * @brief 关闭所有tcp套接字的服务端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_tcp_ip_server() {
    std::vector<int>::iterator it;
    for (it = tcp_ip_socket_server_list.begin();
         it != tcp_ip_socket_server_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        tcp_ip_socket_server_list.erase(it);
    };

    return 1;
}

/**
 * @brief 关闭所有tcp套接字的客户端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_tcp_ip_client() {
    std::vector<int>::iterator it;
    for (it = tcp_ip_socket_client_list.begin();
         it != tcp_ip_socket_client_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        tcp_ip_socket_client_list.erase(it);
    };

    return 1;
}

/**
 * @brief 关闭所有udp套接字的服务端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_udp_ip_server() {
    std::vector<int>::iterator it;
    for (it = udp_ip_socket_server_list.begin();
         it != udp_ip_socket_server_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        udp_ip_socket_server_list.erase(it);
    };

    return 1;
}

/**
 * @brief 关闭所有udp套接字的客户端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_udp_ip_client() {
    std::vector<int>::iterator it;
    for (it = udp_ip_socket_client_list.begin();
         it != udp_ip_socket_client_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        udp_ip_socket_client_list.erase(it);
    };

    return 1;
}