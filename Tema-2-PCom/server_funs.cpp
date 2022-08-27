#include "server.h"

void initialise_sockets(int &tcp_sockfd, int &udp_sockfd) {
    tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(tcp_sockfd < 0, "TCP socket");

    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(udp_sockfd < 0, "UDP socket");
}

void sockaddr_struct_init(sockaddr_in *struct_ptr, int port) {
    memset(struct_ptr, 0, sizeof(sockaddr_in));
    struct_ptr->sin_family = AF_INET;
    struct_ptr->sin_port = htons(port);
    struct_ptr->sin_addr.s_addr = INADDR_ANY;
}

int set_sockets(int tcp_sockfd, int udp_sockfd, fd_set *read_fds) {
    FD_SET(STDIN_FILENO, read_fds);
    FD_SET(tcp_sockfd, read_fds);
    FD_SET(udp_sockfd, read_fds);

    return std::max(tcp_sockfd, udp_sockfd);
}
void bind_sockets(int tcp_sockfd, int udp_sockfd, sockaddr_in tcp_addr,
                  sockaddr_in udp_addr) {
    int binding_res;
    binding_res = bind(tcp_sockfd, (struct sockaddr *) &tcp_addr,
                       sizeof(tcp_addr));
    DIE(binding_res < 0, "TCP bind");

    binding_res = bind(udp_sockfd, (struct sockaddr *) &udp_addr,
                       sizeof(udp_addr));
    DIE(binding_res < 0, "UDP bind");
}

void disable_nagle(int tcp_sockfd) {
    int flag = 1;
    int nagle = setsockopt(tcp_sockfd, IPPROTO_TCP, TCP_NODELAY, &flag,
                           sizeof(int));
    DIE(nagle < 0, "TCP setsockopt failed");
}

bool stdin_exit(fd_set *tmp_fds) {
    if (!FD_ISSET(STDIN_FILENO, tmp_fds)) {
        return false;
    }

    /* Check if it is exit. */
    char buffer[BUFF_LEN];
    BUFF_CLEAR(buffer);
    size_t status = read(STDIN_FILENO, buffer, BUFF_LEN);
    DIE(status < 0, "read failed");

    if (strncmp(buffer, "exit", 4) != 0) {
        std::cerr << "Unknown command." << std::endl;
        return false;
    }

    return true;
}

void close_all(fd_set *read_fds, int tcp_sockfd, int udp_sockfd, int fdmax) {
    /* Tell all the sockets to close. */
    char buffer[BUFF_LEN];
    for (int i = 1; i <= fdmax; i++) {
        if (FD_ISSET(i, read_fds)) {
            if (i == tcp_sockfd || i == udp_sockfd) {
                continue;
            }
            BUFF_CLEAR(buffer);
            sprintf(buffer, "\\$EXIT");
            proto_send(i, buffer, strlen(buffer));
            close(i);
        }
    }

    close(tcp_sockfd);
    close(udp_sockfd);
}

void new_topic(std::map <std::string,ClientMap>& topics, char *topic) {
    topics.insert(std::pair<std::string, ClientMap> (topic, ClientMap()));
}

void send_new_msg(ClientMap& topic_map, cli_msg for_client, ClientMap& connected,
                  ClientMap& saved, char *topic) {
    for (auto &elem: topic_map) {
        Client crt = elem.second;

        if (connected.find(crt.getId()) != connected.end()) {
            proto_send(crt.getClientSocket(), &for_client,
                       sizeof(cli_msg));

        } else {
            if (saved.find(crt.getId()) == saved.end()) {
                continue;
            }

            std::vector <std::string> sfs =
                    saved.at(crt.getId()).getSf();

            if (std::find(sfs.begin(), sfs.end(), topic) != sfs.end()) {
                (saved.at(crt.getId())).addMissed(for_client);
            }
        }
    }
}

bool already_connected(int new_fd, ClientMap& clients,
                       std::string& id) {
    if (clients.find(id) == clients.end()) {
        return false;
    }

    std::cout << "Client " << id << " already connected." << std::endl;
    /* Create a UDP message to the client that he should close. */
    char buffer[BUFF_LEN];
    BUFF_CLEAR(buffer);
    sprintf(buffer, "\\$EXIT");
    proto_send(new_fd, buffer, strlen(buffer));
    close(new_fd);
    return true;
}

int send_missed(ClientMap &saved,
                 ClientMap &connected,
                 std::string& id, int new_fd) {

    /* Check if the ID was previously used and had SF enabled for at least 1 topic. */
    if (saved.find(id) == saved.end()) {
        return -1;
    }

    int missed = 0;

    /* Send everything that are in the queues to the client. */
    while (!(saved.at(id)).getMissed().empty()) {
        cli_msg msg = saved.at(id).getMissed().front();
        (saved.at(id)).popMissed();

        proto_send(new_fd, &msg, sizeof(cli_msg));
        missed++;
    }

    /* Set the new socket and add the client to the connected clients map. */
    saved.at(id).setClientSocket(new_fd);

    connected.insert(std::make_pair(id, saved.at(id)));

    /* Remove the client from the saved clients map. */
    saved.erase(id);

    return missed;
}

std::string welcome(sockaddr_in client_addr, std::string& id) {
    std::string msg = "New client ";
    msg.append(id);
    msg.append(" connected from ");
    msg.append(inet_ntoa(client_addr.sin_addr));
    msg.append(":");
    msg.append(std::to_string(ntohs(client_addr.sin_port)));

    return msg;
}

void unsubscribe(int sockfd, from_tcp msg,
                 std::map <std::string,ClientMap>& topics,
                 ClientMap& connected, std::string& id) {
    /* Check if the topic exists. */
    char buffer[BUFF_LEN];
    BUFF_CLEAR(buffer);
    if (topics.find(msg.topic) == topics.end()) {
        BUFF_CLEAR(buffer);
        sprintf(buffer, "\\$NO_TOPIC");
        proto_send(sockfd, buffer, strlen(buffer));
        return;
    }

    /* Get the topic map of clients and erase the client from it. */
    ClientMap& topic_map = topics.at(msg.topic);
    topic_map.erase(id);
    BUFF_CLEAR(buffer);
    sprintf(buffer, "\\$UNSUBSCRIBED");
    proto_send(sockfd, buffer, strlen(buffer));
}

void subscribe(int sockfd, from_tcp msg,
               std::map <std::string,ClientMap>& topics,
               ClientMap& connected, std::string& id){

    char buffer[BUFF_LEN];
    BUFF_CLEAR(buffer);
    /* Check if the topic exists. */
    if (topics.find(msg.topic) == topics.end()) {
        topics.insert(std::pair<std::string, ClientMap>
                              (msg.topic, ClientMap()));
    }

    /* Get the topic map of clients and insert the current client. */
    ClientMap &topic_map = topics.at(msg.topic);
    /* Check if the client is already subscribed to the topic. */
    if (topic_map.find(id) != topic_map.end()) {
        BUFF_CLEAR(buffer);
        sprintf(buffer, "\\$ALREADY_SUB");
        proto_send(sockfd, &buffer, strlen(buffer));
        return;
    }

    topic_map.insert(std::make_pair(id, connected.at(id)));
    BUFF_CLEAR(buffer);
    sprintf(buffer, "\\$SUBSCRIBED");
    proto_send(sockfd, &buffer, strlen(buffer));

    /* If the client turned on SF, then add the topic to its SF vector. */
    if (msg.sf) {
        connected.at(id).addSf(msg.topic);
    }
}

void msg_from_client(int sockfd, from_tcp msg,
                     std::map <std::string,ClientMap>& topics,
                     ClientMap& connected, std::string& id) {

    switch (msg.type) {
        case UNSUBSCRIBE: {
            /* Check if the topic exists. */
            unsubscribe(sockfd, msg, topics, connected, id);
            break;
        }
        case SUBSCRIBE: {
            subscribe(sockfd, msg, topics, connected, id);
            break;
        }

        default: {
            std:: cerr << "Unknown message type." << std::endl;
        }
    }
}

void client_disconnected(int sockfd, fd_set *read_fds, std::string& id,
                         ClientMap& connected, ClientMap& saved) {
    std::cout << "Client " << id << " disconnected." << std::endl;
    Client client = connected.at(id);

    /* If it has SF for at least one topic, store it. */
    if (!client.getSf().empty()) {
        saved.insert(std::make_pair(id, client));
    }

    /* Delete the client from the connected clients map. */
    connected.erase(id);
    FD_CLR(sockfd, read_fds);
    close(sockfd);
}