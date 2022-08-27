#include "commands.h"


int auth_post(char *host, uint16_t port, std::string& login_cookie, bool auth_type,
              bool& is_connected) {
    if (auth_type == LOGIN && is_connected) {
        std::cout << "You are already logged in, please logout first" <<
        std::endl;
        return EXIT_FAILURE;
    }

    nlohmann::ordered_json user_data = read_user();

    const char* url;
    if (auth_type == REGISTER) {
        url = REGISTER_URL;
    } else if (auth_type == LOGIN) {
        url = LOGIN_URL;
    }

    int sockfd = requester(host, port, url, {}, "POST", "", user_data);
    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    close(sockfd);

    if (strlen(response) == 0) {
        std::cout << "Error: No response from server" << std::endl;
        return EXIT_FAILURE;
    }

    return auth_response_handler(response, login_cookie, auth_type,
                                 is_connected);

}

int enter_library(char* host, uint16_t port, const std::string& login_cookie,
                  std::string& jwt_token, bool is_connected) {
    if (!is_connected) {
        std::cout << "You are not logged in, please login first" << std::endl;
        return EXIT_FAILURE;
    }

    int sockfd = requester(host, port, ENTER_LIBRARY_URL, {login_cookie}, "GET");
    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    close(sockfd);

    if (strlen(response) == 0) {
        std::cout << "Error: No response from server" << std::endl;
        return EXIT_FAILURE;
    }

    return entrance_response_handler(response, jwt_token);
}

int get_summary_info(char *host, uint16_t port, const std::string& jwt_token) {
    if (jwt_token.empty()) {
        std::cout << "You don't have access to the library." << std::endl;
        return EXIT_FAILURE;
    }

    std::string auth = "Bearer " + jwt_token;
    int sockfd = requester(host, port, SUMMARY_INFO_URL, {}, "GET", auth);
    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    close(sockfd);

    if (strlen(response) == 0) {
        std::cout << "Error: No response from server" << std::endl;
        return EXIT_FAILURE;
    }

    return summary_handler(response);
}

int get_details(char *host, uint16_t port, const std::string& jwt_token) {
    if (jwt_token.empty()) {
        std::cout << "You don't have access to the library." << std::endl;
        return EXIT_FAILURE;
    }

    std::string id;
    std::cout << "id=";
    std::cin >> id;

    std::string url_str = ID_NEEDED + id;

    std::string auth = "Bearer " + jwt_token;
    int sockfd = requester(host, port, url_str.c_str(), {}, "GET", auth);
    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    close(sockfd);

    return details_handler(response);
}

int add_book(char *host, uint16_t port, const std::string& jwt_token) {
    if (jwt_token.empty()) {
        std::cout << "You don't have access to the library." << std::endl;
        return EXIT_FAILURE;
    }

    nlohmann::ordered_json json = read_book();

    std::string auth = "Bearer " + jwt_token;
    int sockfd = requester(host, port, ADD_BOOK_URL, {}, "POST", auth, json);

    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    close(sockfd);

    return basic_json_handler(response);
}

int delete_book(char *host, uint16_t port, const std::string& jwt_token) {
    if (jwt_token.empty()) {
        std::cout << "You don't have access to the library." << std::endl;
        return EXIT_FAILURE;
    }

    std::string id;
    std::cout << "id=";
    std::cin >> id;

    std::string url_str = ID_NEEDED + id;

    std::string auth = "Bearer " + jwt_token;
    int sockfd = requester(host, port, url_str.c_str(), {}, "DELETE", auth);

    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    close(sockfd);

    return basic_json_handler(response);
}

int logout(char *host, uint16_t port, const std::string& login_cookie,
           bool& is_connected) {
    if (!is_connected) {
        std::cout << "You are not logged in." << std::endl;
        return EXIT_FAILURE;
    }

    int sockfd = requester(host, port, LOGOUT_URL, {login_cookie}, "GET");

    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    char *response = recv_from_server(sockfd);
    if (strlen(response) == 0) {
        std::cout << "Error: No response from server" << std::endl;
        return EXIT_FAILURE;
    }
    close(sockfd);

    int status = basic_json_handler(response);

    if (status == EXIT_SUCCESS) {
        is_connected = false;
    }

    return status;
}