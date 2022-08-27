/**
 * @file proto.h
 * @author Radu-Andrei Dumitru
 * @brief Prototypes for the functions needed for my protocol implementation.
 */

#ifndef PROTO_H
#define PROTO_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define BUFF_LEN 1600
#define TOPIC_LEN 50

#define UNSUBSCRIBE 0
#define SUBSCRIBE 1

#define BUFF_CLEAR(buffer) memset(buffer, 0, BUFF_LEN)

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

/**
 * @struct from_tcp
 * @brief This structure is for sending / receiving a tcp message.
 * @var from_tcp::type The type of the message. (0 = Unsubscribe, 1 = Subscribe).
 * @var from_tcp::topic The topic to be subscribed or unsubscribed.
 * @var from_tcp::sf Store-and-forward flag. (0 = false, 1 = true).
 */
typedef struct {
    unsigned char type;
    char topic[TOPIC_LEN];
    bool sf;
} from_tcp;

/**
 * @struct from_udp
 * @brief This structure is for receiving an udp message.
 * @var from_udp::topic The topic of the message.
 * @var from_udp::type The type of the payload. (0 = INT, 1 = SHORT_REAL, 2 = FLOAT, 3 = STRING).
 * @var from_udp::payload The payload to be sent.
 */
typedef struct {
    char topic[TOPIC_LEN];
    unsigned char type;
    char payload[1500];
} from_udp;


/**
 * @struct cli_msg
 * @brief This structure is for sending a message to the client.
 * @var cli_msg::recv_msg The message received from the UDP client.
 * @var cli_msg::ip The IP of the UDP client who sent the message.
 * @var cli_msg::port The port of the UDP client who sent the message.
 */
typedef struct {
    from_udp recv_msg;
    struct in_addr ip;
    in_port_t port;
} cli_msg;

/**
 * @brief Sends data to a socket, ensuring that everything is sent.
 * @param sockfd The socket.
 * @param buffer The buffer containing the data.
 */
void proto_send(int sockfd, void *buffer, size_t size);

/**
 * @brief Receives data from a socket, ensuring that everything is received.
 * @param sockfd The socket.
 * @param buffer The buffer that will contain the data.
 *
 * @return The number of bytes received.
 */
size_t proto_recv(int sockfd, char *buffer);

#endif //PROTO_H
