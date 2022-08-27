#include "forms.h"

nlohmann::ordered_json read_user() {
    nlohmann::ordered_json user;
    register_form form{};

    std::cout << "username=";
    std::cin >> form.username;
    user["username"] = form.username;

    std::cout << "password=";
    std::cin >> form.password;
    user["password"] = form.password;

    return user;
}

nlohmann::ordered_json read_book() {
    nlohmann::ordered_json book;
    add_book_form form{};

    std::cout << "title=";
    std::cin >> form.title;
    book["title"] = form.title;

    std::cout << "author=";
    std::cin >> form.author;
    book["author"] = form.author;

    std::cout << "genre=";
    std::cin >> form.genre;
    book["genre"] = form.genre;

    std::cout << "page_count=";
    std::cin >> form.page_count;
    book["page_count"] = form.page_count;

    std::cout << "publisher=";
    std::cin >> form.publisher;
    book["publisher"] = form.publisher;

    return book;
}