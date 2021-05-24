/**
 * @file socket_message.hpp
 * @brief 声明了套接字通信中的消息格式
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-15
 */

#include <stdlib.h>

typedef struct SocketMessage {
    char *buf;
    size_t len;
} SocketMessage;