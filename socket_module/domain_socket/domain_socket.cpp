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

std::vector<int> tcp_domain_socket_server_list;
std::vector<int> udp_domain_socket_server_list;

std::vector<int> tcp_domain_socket_client_list;
std::vector<int> udp_domain_socket_client_list;

///////////////////////////////////////////////////////////////////

/**
 * @brief 初始化一个tcp域套接字服务端
 * @param  socket_addr      域套接字地址
 * @return int 域套接字服务端的socket_fd
 */
int init_tcp_domain_server(const char *const socket_addr) {
    int ret = 0;
    int socket_fd = 0;
    struct sockaddr_un server_addr;

    // 1. 创建套接字
    std::cout << "Socket create...";

    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

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

    // 3. 监听套接字
    std::cout << "Listen socket...";
    ret = listen(socket_fd, MAX_LISTEN_NUM);  // 最大监听数量10

    if (ret < 0) {
        std::cout << "failed!" << std::endl;
        return -1;
    } else {
        std::cout << "success!" << std::endl;
    }

    tcp_domain_socket_server_list.push_back(socket_fd);

    return socket_fd;
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
 * @brief tcp域套接字服务端接收数据，服务端接收完数据主动释放连接
 * @param  socket_fd        服务端的socket_fd
 * @param  msg              数据缓存的指针
 * @return int 如果接收成功，返回接收的字节数;如果接收失败，返回-1
 */
int recv_tcp_domain_msg(const int socket_fd, const SocketMessage *msg) {
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
 * @brief tcp域套接字服务端接收数据，不主动释放链接
 * @param  socket_fd        服务端的socket_fd
 * @param  accept_fd
 * 服务端的accept_fd;如果为0,则建立新的accept_fd;如果不为零，则在此accept_fd上接收数据
 * @param  msg              存放数据的缓存指针
 * @return int
 * 如果接收成功，返回接收的字节数;如果接收失败，关闭accept_fd，返回-1
 */
int recv_tcp_domain_msg_durable(const int &socket_fd, int &accept_fd,
                                const SocketMessage *msg) {
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
 * @param  socket_fd        被关闭tcp域套接服务端的socket_fd
 * @return int 如果关闭成功，返回1;如果关闭失败，返回-1
 */
int close_tcp_domain_server(const int socket_fd) {
    size_t i = 0;
    std::vector<int>::iterator it;
    for (it = tcp_domain_socket_server_list.begin();
         it != tcp_domain_socket_server_list.end(); ++it, ++i) {
        if (*it == socket_fd) break;
    };

    if (i > tcp_domain_socket_server_list.size()) return -1;
    close(socket_fd);

    tcp_domain_socket_server_list.erase(it);

    return 1;
}

/**
 * @brief 关闭一个tcp域套接字客户端
 * @param  socket_fd        被关闭tcp域套接字客户端的socket_fd
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

    // 2. 绑定套接字
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
    std::vector<int>::iterator it;
    for (it = tcp_domain_socket_server_list.begin();
         it != tcp_domain_socket_server_list.end(); ++it) {
        if (close(*it) < 0) return -1;
        tcp_domain_socket_server_list.erase(it);
    };

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