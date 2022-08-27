#ifndef REQUESTS_H
#define REQUESTS_H

#include <string>
#include "single_include/nlohmann/json.hpp"

/**
 * @brief Computes a POST request and stores it in a string.
 * @param host The host to connect to.
 * @param url The url to connect to.
 * @param content_type The content type of the request.
 * @param cookies Cookies to send with the request.
 * @param json The json to send with the request.
 * @param auth The JTW token to send with the request. (optional)
 * @return
 */
std::string compute_post_request(const std::string& host,
                                 const std::string& url,
                                 const std::string& content_type,
                                 const std::vector<std::string>& cookies,
                                 const nlohmann::ordered_json& json,
                                 const std::string &auth = "");

/**
 * @brief Computes a GET request and stores it in a string.
 * @param host The host to connect to.
 * @param url The url to connect to.
 * @param cookies Cookies to send with the request.
 * @param auth The JTW token to send with the request. (optional)
 * @return
 */
std::string compute_get_request(const std::string& host,
                                const std::string& url,
                                const std::vector<std::string>& cookies,
                                const std::string& auth = "");

/**
 * @brief Computes a DELETE request and stores it in a string.
 * @param host The host to connect to.
 * @param url The url to connect to.
 * @param content_type The content type of the request.
 * @param cookies Cookies to send with the request.
 * @param auth The JTW token to send with the request. (optional)
 * @return
 */
std::string compute_delete_request(const std::string& host,
                                   const std::string& url,
                                   const std::string& content_type,
                                   const std::vector<std::string>& cookies,
                                   const std::string& auth);

/**
 * @brief Sends a request to the server.
 * @param host The host to send to.
 * @param port The port to send to.
 * @param request The request to send.
 * @return
 */
int send_request(char *host, uint16_t port, const std::string& request);

/**
 * @brief Computes a POST / GET / DELETE request, opens a connection to the
 * server and sends the request.
 * @param host The host to send to.
 * @param port The port to send to.
 * @param url The url to send to.
 * @param cookies Cookies to send with the request.
 * @param flag The flag to send with the request. (POST / GET / DELETE)
 * @param auth The JTW token to send with the request (optional).
 * @param json An ordered json with the user's credentials. (optional)
 * @return The socket of the connection or -1 if something went wrong.
 */
int requester(char *host, uint16_t port, const char *url,
              const std::vector<std::string>& cookies,
              const std::string& flag,
              const std::string &auth = "",
              const nlohmann::ordered_json& json = nlohmann::ordered_json());

#endif //REQUESTS_H
