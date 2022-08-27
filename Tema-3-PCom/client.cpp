#include "client.h"

int command_caller(const std::string& command, std::string& login_cookie,
                   std::string&jwt_token, bool& is_connected) {
    char *host = strdup(HOST);
    if (host == nullptr) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    int ret_code;
    const std::unordered_map<std::string, std::function<void()>> commands = {
            {"exit", [&]() {ret_code = KILL_CLIENT;}},
            {"register", [&]() {ret_code = auth_post(host, PORT, login_cookie,
                                                     REGISTER,
                                                     is_connected);}},
            {"login", [&]() {ret_code = auth_post(host, PORT, login_cookie,
                                                  LOGIN, is_connected);}},
            {"enter_library", [&]() {ret_code = enter_library(host, PORT,
                                                              login_cookie,
                                                              jwt_token,
                                                              is_connected);}},
            {"get_books", [&]() {ret_code = get_summary_info(host, PORT,
                                                             jwt_token);}},
            {"get_book", [&]() {ret_code = get_details(host, PORT,
                                                       jwt_token);}},
            {"add_book", [&]() {ret_code = add_book(host, PORT, jwt_token);}},
            {"delete_book", [&]() {ret_code = delete_book(host, PORT,
                                                          jwt_token);}},
            {"logout", [&]() {ret_code = logout(host, PORT, login_cookie,
                                                is_connected);}}
    };

    static const auto end = commands.end();
    auto it = commands.find(command);

    if (it == end) {
        std::cout << "Unknown command: " << command << std::endl;
        ret_code = EXIT_FAILURE;
    } else {
        it->second();
    }

    free(host);
    return ret_code;
}

int main() {
    bool is_connected = false;

    std::string login_cookie;
    std::string jwt_token;

    std::string command_result;

    while(true) {
        std::cout << "Waiting for command" << std::endl;
        std::string command;
        std::cin >> command;

        int ret_code = command_caller(command, login_cookie, jwt_token,
                                      is_connected);

        if (ret_code == KILL_CLIENT) {
            break;
        }

        if (ret_code == EXIT_SUCCESS) {
            command_result = "The command: " + command + " was successful!";
            std::cout << command_result << std::endl;
        } else {
            command_result = "The command: " + command + " was unsuccessful!";
            std::cout << command_result << std::endl;
        }
    }

    return 0;
}
