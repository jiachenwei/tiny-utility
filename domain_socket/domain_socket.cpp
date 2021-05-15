/**
 * @file server.cpp
 * @brief
 * 实现了TCP与UDP下的域套接字服务端与客户端，对于用户创建的多个域套接字客户端与服务端可以实现自动内存管理。
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-14
 */

#include "domain_socket.hpp"

#include <iostream>
#include <vector>

typedef struct SocketFileDescriptor {
    int socket_fd;
    int accept_fd;
} SocketFileDescriptor;

std::vector<SocketFileDescriptor> tcp_domain_socket_server_list;
std::vector<int> udp_domain_socket_server_list;

std::vector<int> tcp_domain_socket_client_list;
std::vector<int> udp_domain_socket_client_list;

///////////////////////////////////////////////////////////////////

/**
 * @brief 初始化一个tcp域套接字服务端
 * @param  socket_addr      域套接字地址
 * @return int 域套接字服务端的accept_fd
 */
int init_tcp_domain_server(const char *const socket_addr) {
    int ret = 0;
    struct sockaddr_un server_addr;
    SocketFileDescriptor fd;

    // 1. 创建套接字
    std::cout << "Socket create...";

    fd.socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if (fd.socket_fd < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 移除已有的域套接字路径
    remove(socket_addr);

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = PF_UNIX;
    strcpy(server_addr.sun_path, socket_addr);

    // 绑定套接字
    std::cout << "Binding socket...";
    ret = bind(fd.socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));

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

    ret = fd.accept_fd = accept(fd.socket_fd, NULL, NULL);

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    tcp_domain_socket_server_list.push_back(fd);

    return fd.accept_fd;
}

/**
 * @brief 初始化一个tcp域套接字客户端
 * @param  socket_addr      域套接字地址
 * @return int 域套接字客户端的socket_fd
 */
int init_tcp_domain_client(const char *const socket_addr) {
    int socket_fd;
    int ret = 0;
    struct sockaddr_un server_addr;

    // 1. 创建套接字
    ret = socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    std::cout << "Socket create...";
    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = PF_UNIX;
    strcpy(server_addr.sun_path, socket_addr);

    // 2. 连接套接字
    std::cout << "Connect socket...";
    ret = connect(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    tcp_domain_socket_client_list.push_back(socket_fd);

    return socket_fd;
}

/**
 * @brief tcp域套接字服务端接收数据
 * @param  accpet_fd        服务端的accpet_fd
 * @param  msg              数据缓存的指针
 * @return int 如果接收成功，返回接收的字节数;如果接收失败，返回-1
 */
int recv_tcp_domain_msg(const int accpet_fd, const SocketMessage *msg) {
    bzero(msg->buf, msg->len);
    return recv(accpet_fd, msg->buf, msg->len, 0);
}

/**
 * @brief tcp域套接字客户端发送数据
 * @param  socket_fd        客户端的socket_fd
 * @param  msg              需要发送数据的指针
 * @return int 如果发送成功，返回发送的字节数;如果发送失败，返回-1
 */
int send_tcp_domain_msg(const int socket_fd, const SocketMessage *msg) {
    return send(socket_fd, msg->buf, msg->len, 0);
}

/**
 * @brief 关闭一个tcp域套接字服务端
 * @param  accpet_fd        被关闭服务端的accpet_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_tcp_domain_server(const int accpet_fd) {
    int socket_fd = 0;
    size_t i = 0;
    std::vector<SocketFileDescriptor>::iterator it;
    for (it = tcp_domain_socket_server_list.begin();
         it != tcp_domain_socket_server_list.end(); ++it, ++i) {
        if ((*it).accept_fd == accpet_fd) {
            socket_fd = (*it).accept_fd;
            break;
        }
    }

    if (i > tcp_domain_socket_server_list.size()) return -1;

    close(accpet_fd);
    close(socket_fd);

    tcp_domain_socket_server_list.erase(it);

    return 1;
}

/**
 * @brief 关闭一个tcp域套接字客户端
 * @param  socket_fd        socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_tcp_domain_client(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = tcp_domain_socket_client_list.begin();
         it != tcp_domain_socket_client_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > tcp_domain_socket_client_list.size()) return -1;
    close(socket_fd);

    tcp_domain_socket_client_list.erase(it);

    return 1;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief 初始化一个udp域套接字服务端
 * @param  socket_addr      域套接字地址
 * @return int udp域套接字服务端的socket_fd
 */
int init_udp_domain_server(const char *const socket_addr) {
    int ret = 0;
    struct sockaddr_un server_addr;
    int socket_fd = 0;

    // 1. 创建套接字
    std::cout << "Socket create...";

    socket_fd = socket(PF_UNIX, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 移除已有的域套接字路径
    remove(socket_addr);

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = PF_UNIX;
    strcpy(server_addr.sun_path, socket_addr);

    // 绑定套接字
    std::cout << "Binding socket...";
    ret = bind(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    udp_domain_socket_server_list.push_back(socket_fd);

    return socket_fd;
}

/**
 * @brief 初始化一个udp域套接字客户端
 * @param  socket_addr      域套接字地址
 * @return int udp域套接字客户端的socket_fd
 */
int init_udp_domain_client(const char *const socket_addr) {
    int socket_fd;
    int ret = 0;
    struct sockaddr_un server_addr;

    // 1. 创建套接字
    ret = socket_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
    std::cout << "Socket create...";
    if (ret == -1) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    // 内存区域置0
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = PF_UNIX;
    strcpy(server_addr.sun_path, socket_addr);

    // 2. 连接套接字
    std::cout << "Connect socket...";
    ret = connect(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));

    if (ret == -1) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    udp_domain_socket_client_list.push_back(socket_fd);

    return socket_fd;
}

/**
 * @brief udp域套接字服务端接收数据
 * @param  socket_fd        udp域套接字服务端的socket_fd
 * @param  msg              数据缓存的指针
 * @return int 如果接收成功，返回接收的字节数;如果接收失败，返回-1
 */
int recv_udp_domain_msg(const int socket_fd, const SocketMessage *msg) {
    bzero(msg->buf, msg->len);
    return recvfrom(socket_fd, msg->buf, msg->len, 0, NULL, NULL);
}

/**
 * @brief udp域套接字客户端发送数据
 * @param  socket_fd        udp域套接字客户端的socket_fd
 * @param  msg              数据缓存的指针
 * @return int 如果发送成功，返回发送的字节数;如果发送失败，返回-1
 */
int send_udp_domain_msg(const int socket_fd, const SocketMessage *msg) {
    return sendto(socket_fd, msg->buf, msg->len, 0, NULL, NULL);
}

/**
 * @brief 关闭一个udp域套接字的服务端
 * @param  socket_fd        被关闭的udp域套接字服务端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_udp_domain_server(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = udp_domain_socket_server_list.begin();
         it != udp_domain_socket_server_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > udp_domain_socket_server_list.size()) return -1;
    close(socket_fd);

    udp_domain_socket_server_list.erase(it);

    return 1;
}

/**
 * @brief 关闭一个udp域套接字的客户端
 * @param  socket_fd        被关闭的udp域套接字客户端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_udp_domain_client(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = udp_domain_socket_client_list.begin();
         it != udp_domain_socket_client_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > udp_domain_socket_client_list.size()) return -1;
    close(socket_fd);

    udp_domain_socket_client_list.erase(it);

    return 1;
}

///////////////////////////////////////////////////////////////////

/**
 * @brief 关闭所有tcp域套接字的服务端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_tcp_domain_server() {
    std::vector<SocketFileDescriptor>::iterator it;
    for (it = tcp_domain_socket_server_list.begin();
         it != tcp_domain_socket_server_list.end(); ++it) {
        if (close((*it).accept_fd) < 0 || close((*it).socket_fd) < 0) return -1;
        tcp_domain_socket_server_list.erase(it);
    }

    return 1;
}

/**
 * @brief 关闭所有tcp域套接字的客户端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_tcp_domain_client() {
    std::vector<int>::iterator it;
    for (it = tcp_domain_socket_client_list.begin();
         it != tcp_domain_socket_client_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        tcp_domain_socket_client_list.erase(it);
    };

    return 1;
}

/**
 * @brief 关闭所有udp域套接字的服务端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_udp_domain_server() {
    std::vector<int>::iterator it;
    for (it = udp_domain_socket_server_list.begin();
         it != udp_domain_socket_server_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        udp_domain_socket_server_list.erase(it);
    };

    return 1;
}

/**
 * @brief 关闭所有udp域套接字的客户端
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_all_udp_domain_client() {
    std::vector<int>::iterator it;
    for (it = udp_domain_socket_client_list.begin();
         it != udp_domain_socket_client_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        udp_domain_socket_client_list.erase(it);
    };

    return 1;
}