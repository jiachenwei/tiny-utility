#include <malloc.h>

#include <iostream>

#include "ip_socket/ip_socket.hpp"

#define SERVER_ADDR_ "127.0.0.1"
#define SERVER_PORT_ 1234
#define BUFFER_SIZE_ 10240

using namespace std;

int main() {
    int ret = 0;
    int client_socket_fd = 0;
    SocketMessage msg;
    msg.len = BUFFER_SIZE_;
    msg.buf = (char*)malloc(msg.len);

    cout << "TCP Domain Socket Test." << endl;
    client_socket_fd = init_tcp_ip_client(SERVER_ADDR_, SERVER_PORT_);
    for (size_t i = 0; i < 10; i++) {
        std::cout << "Input messege>>> ";
        fgets(msg.buf, msg.len, stdin);
        ret = send_tcp_ip_msg(client_socket_fd, &msg);
        cout << "(" << ret << ")" << msg.buf << endl;
    }

    ret = close_tcp_ip_client(client_socket_fd);
    cout << "(" << ret << ")" << endl;

    cout << "UDP Domain Socket Test." << endl;
    client_socket_fd = init_udp_ip_client(SERVER_ADDR_, SERVER_PORT_);
    for (size_t i = 0; i < 10; i++) {
        std::cout << "Input messege>>> ";
        fgets(msg.buf, msg.len, stdin);
        ret = send_udp_ip_msg(client_socket_fd, &msg);
        cout << "(" << ret << ")" << msg.buf << endl;
    }
    ret = close_udp_ip_client(client_socket_fd);
    cout << "(" << ret << ")" << endl;
    return 0;
}