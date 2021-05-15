#include <malloc.h>

#include <iostream>

#include "domain_socket.hpp"

#define SOCKET_ADDR_ "./test_domain_socket"
#define BUFFER_SIZE_ 10240

using namespace std;

int main() {
    int ret = 0;
    int client_socket_fd = 0;
    SocketMessage msg;
    msg.len = BUFFER_SIZE_;
    msg.buf = (char*)malloc(msg.len);

    // cout << "TCP Domain Socket Test." << endl;
    // client_socket_fd = init_tcp_domain_client(SOCKET_ADDR_);
    // std::cout << "Input messege>>> ";
    // fgets(msg.buf, msg.len, stdin);
    // ret = send_tcp_domain_msg(client_socket_fd, &msg);
    // cout << "(" << ret << ")" << msg.buf << endl;
    // ret = close_tcp_domain_client(client_socket_fd);
    // cout << "(" << ret << ")" << endl;

    cout << "UDP Domain Socket Test." << endl;
    client_socket_fd = init_udp_domain_client(SOCKET_ADDR_);
    std::cout << "Input messege>>> ";
    fgets(msg.buf, msg.len, stdin);
    ret = send_udp_domain_msg(client_socket_fd, &msg);
    cout << "(" << ret << ")" << msg.buf << endl;
    ret = close_udp_domain_client(client_socket_fd);
    cout << "(" << ret << ")" << endl;
    return 0;
}