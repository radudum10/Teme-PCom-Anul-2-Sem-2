#ifndef COMMANDS_H
#define COMMANDS_H

#include "server_ops.h"
#include "forms.h"
#include "requests.h"
#include "response_handlers.h"

#include "single_include/nlohmann/json.hpp"

#define REGISTER_URL "/api/v1/tema/auth/register"
#define LOGIN_URL "/api/v1/tema/auth/login"
#define ENTER_LIBRARY_URL "/api/v1/tema/library/access"
#define SUMMARY_INFO_URL "/api/v1/tema/library/books"
#define ID_NEEDED "/api/v1/tema/library/books/"
#define ADD_BOOK_URL "/api/v1/tema/library/books"
#define LOGOUT_URL "/api/v1/tema/auth/logout"

/**
 * @brief Function used to login or register a user.
 * @param host Hostname of the server.
 * @param port The port of the server.
 * @param login_cookie A string that will contain the login cookie if the login
 * is successful.
 * @param auth_type LOGIN(true) or REGISTER(false) in order to decide whether
 * to login or register.
 * @param is_connected A boolean that shows whether the client is logged in or
 * not.
 * @return EXIT_SUCCESS(0) if the login/register was successful,
 * EXIT_FAILURE(1) otherwise.
 */
int auth_post(char *host, uint16_t port, std::string& login_cookie, bool auth_type,
              bool& is_connected);

/**
 * @brief Function used to enter the library.
 * @param host The host of the server.
 * @param port The port of the server.
 * @param login_cookie The cookie received after a successful login.
 * @param jwt_token A string that will contain the JWT token after a successful
 * login.
 * @param is_connected A boolean that shows whether the client is logged in or
 * not.
 * @return EXIT_SUCCESS(0) if the user was allowed to enter the library,
 * EXIT_FAILURE(1) otherwise.
 */
int enter_library(char* host, uint16_t port, const std::string& login_cookie,
                  std::string& jwt_token, bool is_connected);

/**
 * @brief Function used to get the summary information of all books in the
 * library.
 * @param host The host of the server.
 * @param port The port of the server.
 * @param jwt_token The JWT token received after a successful login.
 * @return EXIT_SUCCESS(0) if the summary information was received,
 * EXIT_FAILURE(1) otherwise.
 */
int get_summary_info(char *host, uint16_t port, const std::string& jwt_token);

/**
 * @brief Function used to get the details of a book.
 * @param host The host of the server.
 * @param port The port of the server.
 * @param jwt_token The JWT token received after a successful login.
 * @return
 */
int get_details(char *host, uint16_t port, const std::string& jwt_token);

/**
 * @brief Function used to add a book to the library.
 * @param host The host of the server.
 * @param port The port of the server.
 * @param jwt_token The JWT token received after a successful login.
 * @return EXIT_SUCCESS(0) if the book was added, EXIT_FAILURE(1) otherwise.
 */
int add_book(char *host, uint16_t port, const std::string& jwt_token);

/**
 * @brief Function used to delete a book from the library.
 * @param host The host of the server.
 * @param port The port of the server.
 * @param jwt_token The JWT token received after a successful login.
 * @return EXIT_SUCCESS(0) if the book was deleted, EXIT_FAILURE(1) otherwise.
 */
int delete_book(char *host, uint16_t port, const std::string& jwt_token);

/**
 * @brief Function used to logout the user.
 * @param host The host of the server.
 * @param port The port of the server.
 * @param jwt_token The JWT token received after a successful login.
 * @param is_connected A boolean that shows whether the client is logged in or
 * not.
 * @return EXIT_SUCCESS(0) if the user was logged out, EXIT_FAILURE(1) otherwise.
 */
int logout(char *host, uint16_t port, const std::string& login_cookie,
           bool& is_connected);

#endif //COMMANDS_H
