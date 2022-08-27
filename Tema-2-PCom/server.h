/**
 * @file subscriber.h
 * @author Radu-Andrei Dumitru
 * @brief The function prototypes for the server.
 */

#ifndef SERVER_H
#define SERVER_H

#include "proto.h"
#include "Client.h"

#include <algorithm>
#include <map>
#include <list>

#define MAX_CLIENTS 10

typedef std::map<std::string, Client> ClientMap;

/**
 * @brief Initialises the tcp and the udp sockets.
 * @param tcp_sockfd The socket used for TCP.
 * @param udp_sockfd The socket used for UDP.
 */
void initialise_sockets(int &tcp_sockfd, int &udp_sockfd);

/**
 * @brief Initialises a sockaddr struct with AF_INET, INADDR_ANY and the port.
 * @param struct_ptr A pointer to the structure.
 * @param port A port number.
 */
void sockaddr_struct_init(sockaddr_in *struct_ptr, int port);

/**
 * @brief Adds the sockets to the file descriptor set.
 * @param tcp_sockfd The socket used for TCP.
 * @param udp_sockfd The socket used for UDP.
 * @param read_fds The file descriptor set.
 * @return The maximum file descriptor.
 */
int set_sockets(int tcp_sockfd, int udp_sockfd, fd_set *read_fds);

/**
 * Binds the sockets.
 * @param tcp_sockfd The socket used for TCP.
 * @param udp_sockfd The socket used for UDP.
 * @param tcp_addr sockaddr_in structure for the TCP socket.
 * @param udp_addr sockaddr_in structure for the UDP socket.
 */
void bind_sockets(int tcp_sockfd, int udp_sockfd, sockaddr_in tcp_addr,
                  sockaddr_in udp_addr);

/**
 * @brief Disables Nagle's algorithm for a socket.
 * @param sockfd The socket.
 */
void disable_nagle(int tcp_sockfd);

/**
 * @brief Checks if there is input from stdin and if it is exit.
 * @param read_fds The file descriptor set.
 * @return true if exit is received.
 */
bool stdin_exit(fd_set *read_fds);

/**
 * @brief Closes all the sockets.
 * @param read_fds The file descriptor set.
 * @param tcp_sockfd The socket used for TCP.
 * @param udp_sockfd The socket used for UDP.
 * @param fdmax The maximum file descriptor.
 * @return True if exit was received.
 */
void close_all(fd_set *read_fds, int tcp_sockfd, int udp_sockfd, int fdmax);

/**
 * @brief If a new client tries to connect, but the ID is already in use, it tells it
 * to exit.
 * @param new_fd The new client's socket.
 * @param clients The map of connected clients.
 * @param id The new client's ID.
 * @return
 */
bool already_connected(int new_fd, std::map<std::string, Client>& clients,
                       std::string& id);

/**
 * @brief When a client reconnects, sends it what messages it has missed.
 * @param saved The map of saved clients.
 * @param connected The map of connected clients.
 * @param id The client's ID.
 * @param new_fd The new client's socket.
 * @return False if the client is not connected.
 */
int send_missed(std::map<std::string, Client> &saved,
                 std::map<std::string, Client> &connected,
                 std::string& id, int new_fd);

/**
 * @brief Builds a string to inform the client that it is connected.
 * @param client_addr The client's address.
 * @param id The client's ID.
 * @return -1 if the client was not stored. Otherwise, returns the number of
 * missed messages that were sent.
 */
std::string welcome(sockaddr_in client_addr, std::string& id);

/**
 * @brief Inserts a new topic.
 * @param topics The topic map.
 * @param topic The new topic's name.
 */
void new_topic(std::map <std::string,ClientMap>& topics, char *topic);

/**
 * @brief When a new message is received from UDP, it sends it to all the
 * subscribers.
 * @param topic_map The topic map.
 * @param for_client The message for the client.
 * @param connected The map of connected clients.
 * @param saved The map of saved clients.
 * @param topic The topic's name.
 */
void send_new_msg(ClientMap& topic_map, cli_msg for_client, ClientMap& connected,
                  ClientMap& saved, char *topic);

/**
 * @brief Receives a message from a client and subscribe / unsubscribe.
 * @param sockfd The client's socket.
 * @param msg The message.
 * @param topics The topic map.
 * @param connected The map of connected clients.
 * @param id The client's ID.
 */
void msg_from_client(int sockfd, from_tcp msg,
                     std::map <std::string,std::map<std::string, Client>>& topics,
                     std::map<std::string, Client>& connected, std::string& id);

/**
 * @brief When a client disconnects, it displays a message and saves the client
 * in the saved map if it has at least one topic with SF.
 * @param sockfd The client's socket.
 * @param read_fds The file descriptor set.
 * @param id The client's ID.
 * @param connected The map of connected clients.
 * @param saved The map of saved clients.
 */

void client_disconnected(int sockfd, fd_set *read_fds, std::string& id,
                         ClientMap& connected, ClientMap& saved);

#endif //SERVER_H
