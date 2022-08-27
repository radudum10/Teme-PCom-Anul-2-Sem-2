#include "server_ops.h"
#include "buffer.h"

int open_connection(char *host, uint16_t port) {
    struct sockaddr_in serv_addr{};
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        std::cout << "Error opening socket" << std::endl;
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_aton(host, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        return -1;
    }

    return sockfd;
}

size_t send_to_server(int sockfd, char *message)
{
    ssize_t bytes_sent = 0;
    size_t bytes_remaining = strlen(message);

    while (bytes_remaining > 0) {
        ssize_t status = write(sockfd, message + bytes_sent, bytes_remaining);

        if (status < 0) {
            std::cerr << "Error sending message to server" << std::endl;
            return -1;
        }
        bytes_remaining -= status;
        bytes_sent += status;
    }

    return bytes_sent;
}

 char *recv_from_server(int sockfd) {
    char response[BUFLEN];
    buffer buffer = buffer_init();

    size_t header_end = 0;
    size_t bytes_recv;

    size_t content_length = 0;

    do {
        bytes_recv = read(sockfd, response, BUFLEN);

        if (bytes_recv < 0) {
            std::cerr << "Error receiving message from server" << std::endl;
            exit(1);
        }

        if (bytes_recv == 0) {
            break;
        }

        buffer_add(&buffer, response, bytes_recv);

        header_end = buffer_find(&buffer, HEADER_TERMINATOR,
                                 HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            size_t content_length_start =
                    buffer_find_insensitive(&buffer,CONTENT_LENGTH,
                                            CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start,
                                    nullptr, 10);
            break;
        }
    } while(true);

    size_t total = content_length + header_end;

    while (buffer.size < total) {
        bytes_recv = read(sockfd, response, BUFLEN);

        if (bytes_recv < 0) {
            std::cerr << "Error receiving message from server" << std::endl;
            exit(1);
        }

        if (bytes_recv == 0) {
            break;
        }

        buffer_add(&buffer, response, bytes_recv);
    }

    buffer_add(&buffer, "", 1);

    return buffer.data;
}