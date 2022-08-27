#ifndef FORMS_H
#define FORMS_H

#include <string>
#include <iostream>
#include "single_include/nlohmann/json.hpp"

typedef struct register_form {
    std::string username;
    std::string password;
} register_form;

typedef struct add_book_form {
    std::string title;
    std::string author;
    std::string genre;
    unsigned int page_count;
    std::string publisher;
} add_book_form;

/**
 * @brief Reads the user's credentials and stores it in a json.
 * @return An ordered json with the user's credentials.
 */
nlohmann::ordered_json read_user();

/**
 * @brief Function used to read a new book.
 * @return A json with the new book's information.
 */
nlohmann::ordered_json read_book();

#endif //FORMS_H
