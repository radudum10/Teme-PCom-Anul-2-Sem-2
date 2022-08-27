#include "requests.h"
#include "server_ops.h"

#include <iostream>

void compute_string(std::string& message, std::string& line) {
    message += line + "\r\n";
}

std::string compute_post_request(const std::string& host,
                                 const std::string& url,
                                 const std::string& content_type,
                                 const std::vector<std::string>& cookies,
                                 const nlohmann::ordered_json& json,
                                 const std::string &auth) {
    std::string message;
    std::string line;

    line = "POST " + url + " HTTP/1.1";
    compute_string(message, line);

    line = "Host: " + host;
    compute_string(message, line);

    line = "Content-Type: " + content_type;
    compute_string(message, line);

    line = "Content-Length: " + std::to_string(json.dump().length());
    compute_string(message, line);

    for (auto& cookie: cookies) {
        line = "Cookie: " + cookie;
        compute_string(message, line);
    }

    if (!auth.empty()) {
        line = "Authorization: " + auth;
        compute_string(message, line);
    }

    message += "\r\n";
    message += json.dump();

    return message;
}

std::string compute_get_request(const std::string& host,
                                const std::string& url,
                                const std::vector<std::string>& cookies,
                                const std::string& auth) {
    std::string message;
    std::string line;

    line = "GET " + url + " HTTP/1.1";
    compute_string(message, line);

    line = "Host: " + host;
    compute_string(message, line);

    for (auto& cookie: cookies) {
        line = "Cookie: " + cookie;
        if (cookie == cookies.back()) {
            line += "; ";
        }
        compute_string(message, line);
    }

    if (!auth.empty()) {
        line = "Authorization: " + auth;
        compute_string(message, line);
    }

    message += "\r\n";

    return message;
}

std::string compute_delete_request(const std::string& host,
                                   const std::string& url,
                                   const std::string& content_type,
                                   const std::vector<std::string>& cookies,
                                   const std::string& auth) {
    std::string message;
    std::string line;

    line = "DELETE " + url + " HTTP/1.1";
    compute_string(message, line);

    line = "Host: " + host;
    compute_string(message, line);

    for (auto& cookie: cookies) {
        line = "Cookie: " + cookie;
        if (cookie == cookies.back()) {
            line += "; ";
        }
        compute_string(message, line);
    }

    if (!auth.empty()) {
        line = "Authorization: " + auth;
        compute_string(message, line);
    }

    message += "\r\n";

    return message;
}

int send_request(char *host, uint16_t port, const std::string& request) {
    char *request_c = strdup(request.c_str());
    if (request_c == nullptr) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    int sockfd = open_connection(host, port);
    if (sockfd < 0) {
        std::cout << "Error: Could not connect to server" << std::endl;
        return EXIT_FAILURE;
    }

    if (send_to_server(sockfd, request_c) < 0) {
        std::cout << "Error: Could not send request to server" << std::endl;
        return EXIT_FAILURE;
    }


    free(request_c);

    return sockfd;
}

int requester(char *host, uint16_t port, const char *url,
              const std::vector<std::string>& cookies,
              const std::string& flag,
              const std::string &auth,
              const nlohmann::ordered_json& json) {
    std::string req;

    if (flag == "POST") {
        req = compute_post_request(std::string(host), url,
                                        "application/json", cookies, json,
                                        auth);
    } else if (flag == "GET") {
        req = compute_get_request(std::string(host), url, cookies, auth);
    } else if (flag == "DELETE") {
        req = compute_delete_request(std::string(host), url,
                                        "application/json", cookies, auth);
    } else {
        std::cout << "UNKNOWN FLAG FOR REQUESTER" << std::endl;
        return -1;
    }

    int sockfd = send_request(host, port, req);

    return sockfd;
}