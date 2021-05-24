#include <iostream>

#include "ip_socket/ip_socket.hpp"
#include "malloc.h"

#define SERVER_PORT_ 1234
#define BUFFER_SIZE_ 10240

using namespace std;

int main() {
    int ret = 0;
    int server_socket_fd = 0, server_accept_fd = 0;
    SocketMessage msg;
    msg.len = BUFFER_SIZE_;
    msg.buf = (char*)malloc(msg.len);

    cout << "TCP Domain Socket Test." << endl;
    server_socket_fd = init_tcp_ip_server(SERVER_PORT_);
    while (1) {
        ret = recv_tcp_ip_msg_durable(server_socket_fd, server_accept_fd, &msg);
        cout << "(" << ret << ")"
             << ":Received messege from remote: " << msg.buf << endl;
    }
    ret = close_tcp_ip_server(server_socket_fd);
    cout << "(" << ret << ")" << endl;

    cout << "UDP Domain Socket Test." << endl;
    server_socket_fd = init_udp_ip_server(SERVER_PORT_);
    while (1) {
        ret = recv_udp_ip_msg(server_socket_fd, &msg);
        cout << "(" << ret << ")"
             << ":Received messege from remote: " << msg.buf << endl;
    }
    ret = close_udp_ip_server(server_socket_fd);
    cout << "(" << ret << ")" << endl;

    return 0;
}