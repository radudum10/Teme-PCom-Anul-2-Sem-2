/**
 * @file subscriber.h
 * @author Radu-Andrei Dumitru
 * @brief Contains the client class and its methods prototypes.
 */
#ifndef CLIENT_H
#define CLIENT_H

#include "proto.h"

#include <string>
#include <list>
#include <queue>
#include <map>

/**
 * @brief The class used as template for a client.
 * @details The class is used as template for a client.
 */
class Client {
public:
    Client(int clientSocket, std::string id);

    /* Getters and setters */
    std::string getId() const;

    int getClientSocket() const;

    std::vector<std::string> &getSf();

    std::queue<cli_msg> getMissed() const;

    void setClientSocket(int clientSocket);

    /**
     * @brief If a subscription with SF 1 is received, the topic is added to
     * the SF vector.
     * @param topic The topic received.
     */
    void addSf(const std::string& topic);

    /**
     * @brief If the client is disconnected, add it to its missed queue.
     * @param msg
     */
    void addMissed(cli_msg msg);

    void popMissed();

private:
    int client_socket;
    std::string id;
    std::vector<std::string> sf;
    std::queue<cli_msg> missed;
};


#endif // CLIENT_H
