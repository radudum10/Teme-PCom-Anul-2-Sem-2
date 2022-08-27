#ifndef SERVER_OPS_H
#define SERVER_OPS_H

#include <iostream>
#include <cstring>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFLEN 4096

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)

#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

int open_connection(char *host, uint16_t port);

size_t send_to_server(int sockfd, char *message);

char *recv_from_server(int sockfd);

#endif //SERVER_OPS_H
