#include <iostream>

#include "domain_socket.hpp"
#include "malloc.h"

#define SOCKET_ADDR_ "./test_domain_socket"
#define BUFFER_SIZE_ 10240

using namespace std;

int main() {
    int ret = 0;
    int server_accept_fd = 0;
    SocketMessage msg;
    msg.len = BUFFER_SIZE_;
    msg.buf = (char*)malloc(msg.len);

    // cout << "TCP Domain Socket Test." << endl;
    // server_accept_fd = init_tcp_domain_server(SOCKET_ADDR_);
    // ret = recv_tcp_domain_msg(server_accept_fd, &msg);
    // cout << "(" << ret << ")"
    //      << ":Received messege from remote: " << msg.buf << endl;
    // ret = close_tcp_domain_server(server_accept_fd);
    // cout << "(" << ret << ")" << endl;

    cout << "UDP Domain Socket Test." << endl;
    server_accept_fd = init_udp_domain_server(SOCKET_ADDR_);
    ret = recv_udp_domain_msg(server_accept_fd, &msg);
    cout << "(" << ret << ")"
         << ":Received messege from remote: " << msg.buf << endl;
    ret = close_udp_domain_server(server_accept_fd);
    cout << "(" << ret << ")" << endl;

    return 0;
}