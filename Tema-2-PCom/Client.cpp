#include "Client.h"
#include <utility>

Client::Client(int clientSocket, std::string id) : client_socket(clientSocket),
               id(std::move(id)) {}

int Client::getClientSocket() const {
    return client_socket;
}

std::string Client::getId() const {
    return id;
}


void Client::setClientSocket(int clientSocket) {
    client_socket = clientSocket;
}

std::vector<std::string> &Client::getSf() {
    return sf;
}

void Client::addSf(const std::string& topic) {
    sf.push_back(topic);
}

void Client::addMissed(cli_msg msg) {
    missed.push(msg);
}

std::queue<cli_msg> Client::getMissed() const {
    return missed;
}

void Client::popMissed() {
    missed.pop();
}

