#include "subscriber.h"
#include "proto.h"

int main(int argc, char *argv[]) {
    setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    if (argc != 4) {
        std::cout << "Usage: subscriber <ID> <SERVER_IP> <PORT>" << std::endl;
        return 1;
    }

    int sockfd;
    ssize_t status;
    struct sockaddr_in serv_addr{};
    fd_set read_fds, tmp_fds;

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(strtol(argv[3], nullptr, 10));
    status = inet_aton(argv[2], &serv_addr.sin_addr);
    DIE(status == 0, "inet_aton");

    status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    DIE(status < 0, "connect");

    FD_SET(sockfd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    proto_send(sockfd, argv[1], strlen(argv[1]));

    char buffer[BUFF_LEN];
    while (true) {
        tmp_fds = read_fds;

        status = select(sockfd + 1, &tmp_fds, nullptr, nullptr, nullptr);
        DIE(status < 0, "select");

        /* Check if there is input from stdin. */
        if (recv_from_stdin(sockfd, &tmp_fds)) {
            break;
        }

        if (FD_ISSET(sockfd, &tmp_fds)) {
            /* Receive a message from the server. */
            size_t size = proto_recv(sockfd, buffer);

            if (size < sizeof(cli_msg)) {
                std::string maybe_op = std::string(buffer);
                int code = -1;
                get_srv_opcode(maybe_op, code);

                if (code != - 1) { // if the message is a server opcode
                    opcode_received(code);
                    continue;
                }
            }

            cli_msg new_msg;
            memcpy(&new_msg, &buffer, sizeof(cli_msg));

            identify_msg(new_msg);
        }
    }

    close(sockfd);
    return 0;
}