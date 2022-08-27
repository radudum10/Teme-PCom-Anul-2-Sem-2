#include "proto.h"

void proto_send(int sockfd, void *buffer, size_t size) {
    ssize_t status = send(sockfd, &size, sizeof(size_t), 0);
    DIE(status < 0, "send size");

    ssize_t bytes_sent = 0;
    auto bytes_to_send = size;
    while (bytes_sent < size) {
        status = send(sockfd, (char *)buffer + bytes_sent, bytes_to_send, 0);
        DIE(status < 0, "send id");
        bytes_sent += status;
        bytes_to_send -= status;
    }
}

size_t proto_recv(int sockfd, char *buffer) {
    size_t size = 0;
    ssize_t status = recv(sockfd, &size, sizeof(size_t), 0);
    DIE(status < 0, "recv size");

    size_t bytes_recv = 0;
    auto bytes_to_recv = size;

    BUFF_CLEAR(buffer);

    while (bytes_recv < size) {
        status = recv(sockfd, buffer + bytes_recv, bytes_to_recv, 0);
        DIE(status < 0, "recv");

        bytes_recv += status;
        bytes_to_recv -= status;
    }

    return size;
}
