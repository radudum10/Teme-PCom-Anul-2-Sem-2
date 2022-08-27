/**
 * @file subscriber.h
 * @author Radu-Andrei Dumitru
 * @brief Function prototypes for those needed by the subscriber.
 */

#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "proto.h"

#include <valarray>
#include <unordered_map>
#include <functional>

#define EXIT_CODE 0
#define NO_TOPIC 404
#define SUBSCRIBED 100
#define UNSUBSCRIBED 101
#define ALREADY_SUB 200

#define INT_TYPE 0
#define SHORT_TYPE 1
#define FLOAT_TYPE 2
#define STRING_TYPE 3

/**
 * @brief The strings that can be received from the server are mapped to some
 * INT values.
 * @param opcode The string received from the server.
 * @param &code A reference to the variable that will be set to the mapped value.
 */
void get_srv_opcode(std::string &opcode, int &code);

/**
 * @brief Checks if there is input from stdin and treats it accordingly, using
 * send_subscription and remove_subscription.
 * @param sockfd The socket for the server connection.
 * @param tmp_fds The temporary file descriptors
 * @return true if 'exit' was received, false otherwise.
 */
bool recv_from_stdin(int sockfd, fd_set *tmp_fds);

/**
 * @brief After receiving a code from the server, treats it accordingly.
 * @param code The received code.
 */
void opcode_received(int code);

/**
 * @brief After receiving a message from the server, treats it according to the
 * type of the message.
 * @param new_msg The received message.
 */
void identify_msg(cli_msg new_msg);

#endif //SUBSCRIBER_H
