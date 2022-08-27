#include "response_handlers.h"

int login_cookie_handler(char *response, std::string& login_cookie,
                         bool& is_connected) {
    char *cookie_start = strstr(response, "Set-Cookie: ");

    if (!cookie_start) {
        std::cout << "Login failed.\n";
        free(response);
        return EXIT_FAILURE;
    }

    char *cookie_end = strstr(cookie_start, ";");
    if (!cookie_end) {
        std::cout << "Login failed.\n";
        free(response);
        return EXIT_FAILURE;
    }

    login_cookie = std::string(cookie_start + 12,
                               cookie_end - cookie_start - 12);
    is_connected = true;

    free(response);

    return EXIT_SUCCESS;
}

char *extract_json_response(char *response) {
    return strstr(response, "\r\n\r\n");
}

int auth_response_handler(char *response, std::string &login_cookie,
                          bool auth_type, bool& is_connected) {
    char *data = extract_json_response(response);
    if (!data) {
        std::cout << "Respones extracting failed" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    nlohmann::json json_response;
    try {
        json_response = nlohmann::json::parse(data);
    } catch(nlohmann::json::parse_error &ex) {
        if (auth_type == LOGIN) {
            return login_cookie_handler(response, login_cookie, is_connected);
        }
        free(response);
        return EXIT_SUCCESS;
    }

    if (json_response.contains("error") && auth_type == REGISTER) {
        std::cout << "The username is already taken" << std::endl;
        free(response);
        return EXIT_FAILURE;
    } else if (json_response.contains("error") && auth_type == LOGIN) {
        std::cout << "Wrong username or password" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    free(response);

    return EXIT_SUCCESS;
}

int parse_json(nlohmann::json& json_response, char *data) {
    try {
        json_response = nlohmann::json::parse(data);
    } catch(nlohmann::json::parse_error &ex) {
        std::cout << "Invalid JSON from server. Please try again" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int entrance_response_handler(char *response, std::string& jwt_token) {
    char *data = extract_json_response(response);
    if (!data) {
        std::cout << "Respones extracting failed" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    nlohmann::json json_response;
    if (parse_json(json_response, data) == EXIT_FAILURE) {
        free(response);
        return EXIT_FAILURE;
    }

    if (!json_response.contains("token")) {
        std::cout << "Invalid credentials" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    jwt_token = json_response["token"];

    free(response);

    return EXIT_SUCCESS;
}

int summary_handler(char *response) {
    char *data = extract_json_response(response);
    if (!data) {
        std::cout << "Respones extracting failed" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    nlohmann::json json_response;
    if (parse_json(json_response, data) == EXIT_FAILURE) {
        free(response);
        return EXIT_FAILURE;
    }

    if (!json_response.is_array()) {
        if (json_response.contains("error")) {
            std::cout << "Token error!" << std::endl;
        }
        free(response);
        return EXIT_FAILURE;
    }

    std::cout << json_response.dump(4) << std::endl;

    free(response);

    return EXIT_SUCCESS;
}

int details_handler(char *response) {
    char *data = extract_json_response(response);
    if (!data) {
        std::cout << "Respones extracting failed" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    nlohmann::json json_response;
    if (parse_json(json_response, data) == EXIT_FAILURE) {
        free(response);
        return EXIT_FAILURE;
    }

    if (json_response.contains("error")) {
        std::cout << json_response["error"].dump() << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    std::cout << json_response.dump(4) << std::endl;

    free(response);

    return EXIT_SUCCESS;
}

int basic_json_handler(char *response) {
    char *data = extract_json_response(response);
    if (!data) {
        std::cout << "Respones extracting failed" << std::endl;
        free(response);
        return EXIT_FAILURE;
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(data);
        if (json_response.contains("error")) {
            std::cout << json_response["error"].dump() << std::endl;
            free(response);
            return EXIT_FAILURE;
        }
    } catch(nlohmann::json::parse_error &ex) {
        free(response);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

