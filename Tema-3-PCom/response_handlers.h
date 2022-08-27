#ifndef RESPONSE_HANDLERS_H
#define RESPONSE_HANDLERS_H

#include "single_include/nlohmann/json.hpp"
#include <iostream>

#define LOGIN true
#define REGISTER false

/**
 * @brief After a successful login, a cookie is received and this function
 * extracts it from the response.
 * @param response The response from the server.
 * @param login_cookie The string where the cookie will be stored.
 * @param is_connected A boolean that will be set to true, because the user
 * is now connected.
 * @return EXIT_FAILURE(1) if something went wrong with the char pointers
 * operations or EXIT_SUCCESS(0) if everything went fine.
 */
int login_cookie_handler(char *response, std::string& login_cookie,
                         bool& is_connected);

/**
 * @brief Extracts the body (possibly a json) from a response.
 * @param response The response from the server.
 * @return A pointer to the body of the response.
 */
char *extract_json_response(char *response);

/**
 * @brief Extracts the data from the response and handles it.
 * @param response The response from the server.
 * @param login_cookie A string where the cookie will be stored (in case of a
 * successful login).
 * @param auth_type LOGIN (true) or REGISTER (false) in order to decide whether
 * to handle a login or a register response.
 * @param is_connected A boolean that will be set to true, in case of a
 * successful login.
 * @return EXIT_FAILURE(1) in case of parsing errors / user already connected /
 * invalid credentials or EXIT_SUCCESS(0) if everything went fine.
 */
int auth_response_handler(char *response, std::string &login_cookie,
                          bool auth_type, bool& is_connected);

/**
 * @brief Parses a json from a response.
 * @param json_response A reference to a json object that will contain the
 * parsed json.
 * @param data The data that will be parsed.
 * @return EXIT_FAILURE (1) if something went wrong in parsing or EXIT_SUCCESS
 * (0) if everything went fine.
 */
int parse_json(nlohmann::json& json_response, char *data);
/**
 * @brief Computes the response of the server after an attempt to enter the
 * library.
 * @param response The response from the server.
 * @param jwt_token The token received if the authentication was successful.
 * @return EXIT_FAILURE (1) if something went wrong, EXIT_SUCCESS (0)
 * otherwise.
 */
int entrance_response_handler(char *response, std::string& jwt_token);

/**
 * @brief Computes the response of the server after an attempt to get the
 * summary information of all books. If the user has access to the library,
 * it prints a list of books, otherwise it prints an error message.
 * @param response The response from the server.
 * @return EXIT_FAILURE (1) if something went wrong, EXIT_SUCCESS (0)
 * otherwise.
 */
int summary_handler(char *response);

/**
 * @brief Computes the response of the server after an attempt to get the
 * details of a book. If the user has access to the library and the book
 * exists, it prints the details of the book, otherwise it prints an error.
 * @param response The response from the server.
 * @return EXIT_FAILURE (1) if something went wrong, EXIT_SUCCESS (0)
 * otherwise.
 */
int details_handler(char *response);

/**
 * @brief Computes the response of the server after an attempt to add a new
 * book / delete a book.
 * @param response The response from the server.
 * @return EXIT_FAILURE (1) if something went wrong, EXIT_SUCCESS (0)
 * otherwise.
 */
int basic_json_handler(char *response);

#endif //RESPONSE_HANDLERS_H
