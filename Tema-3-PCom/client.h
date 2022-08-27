#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <cstring>
#include <cstdint>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server_ops.h"
#include "forms.h"
#include "requests.h"
#include "commands.h"

#define HOST "34.241.4.235"
const uint16_t PORT = 8080;

#define KILL_CLIENT (-2)

/**
 * @brief Using a map with some lambdas, the function needed for the command is
 * called.
 * @param command A string containing the command.
 * @param login_cookie A string that will contain the login cookie if the user
 * wants to log in.
 * @param is_connected A boolean that will be set to true if the user is
 * already connected.
 * @return KILL_CLIENT(-2) if the user wants to quit, EXIT_FAILURE(-1) if the
 * given command couldn't be computed as wished, EXIT_SUCCESS(0) otherwise.
 */
int command_caller(const std::string& command, std::string&login_cookie,
                   std::string&jwt_token, bool& is_connected);


#endif //CLIENT_H
