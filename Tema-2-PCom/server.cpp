#include "server.h"
#include "Client.h"

int main(int argc, char *argv[]) {
    setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    fd_set read_fds, tmp_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    /* Initialise the sockets. */
    int tcp_sockfd, udp_sockfd;
    initialise_sockets(tcp_sockfd, udp_sockfd);
    tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int fdmax = set_sockets(tcp_sockfd, udp_sockfd, &read_fds);

    /* Disable the Nagle algorithm. */
    disable_nagle(tcp_sockfd);

    /* Set the sockaddr structs. */
    int port_no = (int) strtol(argv[1], nullptr, 10);
    DIE (port_no <= 0, "Port number is really wrong");

    struct sockaddr_in serv_addr{}, udp_client_addr{};
    sockaddr_struct_init(&serv_addr, port_no);
    sockaddr_struct_init(&udp_client_addr, port_no);

    /* Bind the sockets. */
    bind_sockets(tcp_sockfd, udp_sockfd, serv_addr, udp_client_addr);

    /* Listen to the socket. */
    int listen_result = listen(tcp_sockfd, MAX_CLIENTS);
    DIE(listen_result < 0, "listen failed");

    char buffer[BUFF_LEN];
    size_t status;

    std::map <std::string, Client> connected_clients;
    std::map <std::string, Client> saved_clients;
    std::map <std::string, ClientMap> topics;

    while(true) {
        tmp_fds = read_fds;
        status = select(fdmax + 1, &tmp_fds, nullptr, nullptr,
                        nullptr);
        DIE(status < 0, "select failed");

        /* Check if there is input on stdin. */
        if (stdin_exit(&tmp_fds)) {
            close_all(&read_fds, tcp_sockfd, udp_sockfd, fdmax);
            break;
        }

        for (int i = 1; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == udp_sockfd) {
                    /* Receive a message from the UDP socket. */
                    socklen_t udp_len = sizeof(udp_client_addr);
                    from_udp new_msg;
                    memset(&new_msg, 0, sizeof(from_udp));
                    status = (int) recvfrom(udp_sockfd, &new_msg, sizeof(from_udp), 0,
                                            (struct sockaddr *) &udp_client_addr,
                                            &udp_len);

                    DIE(status < 0, "UDP recvfrom failed");

                    /* Check if the topic exists, if not create it. */
                    if (topics.find(new_msg.topic) == topics.end()) {
                        new_topic(topics, new_msg.topic);
                        continue;
                    }

                    /* Send the message to all the subscribers. */
                    ClientMap &topic_map = topics[new_msg.topic];
                    cli_msg for_client;
                    for_client.ip = udp_client_addr.sin_addr;
                    for_client.port = udp_client_addr.sin_port;
                    memcpy(&(for_client.recv_msg), &new_msg, sizeof(from_udp));

                    send_new_msg(topic_map, for_client, connected_clients,
                                 saved_clients, new_msg.topic);

                } else if (i == tcp_sockfd) {
                    /* Accept a new connection. */
                    struct sockaddr_in client_addr{};
                    socklen_t client_len = sizeof(client_addr);

                    int new_fd = accept(tcp_sockfd, (struct sockaddr *) &client_addr,
                                         &client_len);
                    DIE(new_fd < 0, "accept failed");

                    size_t size = proto_recv(new_fd, buffer);
                    if (size == 0) {
                        continue;
                    }

                    /* The client sends the ID first. */
                    std::string id_stringed(buffer);

                    /* Check if the ID already exists. */
                    if (already_connected(new_fd, connected_clients, id_stringed)) {
                        continue;
                    }

                    /* Add the new client socket to the set of file descriptors. */
                    FD_SET(new_fd, &read_fds);
                    fdmax = std::max(fdmax, new_fd);
                    disable_nagle(new_fd);

                    /* If it is a reconnection, then send it everything he missed from
                     * the topic he had SF on.
                     */
                    int missed = send_missed(saved_clients, connected_clients,
                                             id_stringed, new_fd);

                    if (missed == -1) {
                        /* If the client was not stored, create a new client. */
                        Client new_client(new_fd, id_stringed);
                        connected_clients.insert(std::make_pair(id_stringed, new_client));
                    }

                    std::cout << welcome(client_addr, id_stringed) << std::endl;
                } else {
                    /* Receive a message from a client. */
                    size_t size = proto_recv(i, buffer);

                    from_tcp msg;
                    memset(&msg, 0, sizeof(from_tcp));
                    memcpy(&msg, buffer, size);

                    std::string id;
                    /* Find the client in the connected clients map by the socket. */
                    for (auto &elem: connected_clients) {
                        if (elem.second.getClientSocket() == i) {
                            id = elem.first;
                            break;
                        }
                    }

                    /* If the message is empty, the client has disconnected. */
                    if (size == 0) {
                        client_disconnected(i, &read_fds, id, connected_clients,
                                            saved_clients);
                        continue;
                    }

                    msg_from_client(i, msg, topics, connected_clients, id);
                }
            }
        }
    }

    return 0;
}
