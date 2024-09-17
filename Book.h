#pragma once
#include <iostream>
#include <json/json.h>
#include <string>

using namespace std;

class Book {
public:
    std::string isbn;
    std::string name;
    std::string author;
    std::string category;
    std::string price;
    std::string quantity;

    // Default constructor
    Book() {
    }

    // Parameterized constructor
    Book(const std::string &isbn, const std::string &name, const std::string &author,
         const std::string &category, const std::string &price, const std::string &quantity)
        : isbn(isbn), name(name), author(author), category(category), price(price), quantity(quantity) {
    }

    // Copy constructor
    Book(const Book &other)
        : isbn(other.isbn), name(other.name), author(other.author), category(other.category),
          price(other.price), quantity(other.quantity) {
    }

    // Move constructor
    Book(Book &&other) noexcept
        : isbn(std::move(other.isbn)), name(std::move(other.name)), author(std::move(other.author)),
          category(std::move(other.category)), price(std::move(other.price)), quantity(std::move(other.quantity)) {
    }

    // Copy assignment operator
    Book& operator=(const Book &other) {
        if (this != &other) {
            isbn = other.isbn;
            name = other.name;
            author = other.author;
            category = other.category;
            price = other.price;
            quantity = other.quantity;
        }
        return *this;
    }

    // Move assignment operator
    Book& operator=(Book &&other) noexcept {
        if (this != &other) {
            isbn = std::move(other.isbn);
            name = std::move(other.name);
            author = std::move(other.author);
            category = std::move(other.category);
            price = std::move(other.price);
            quantity = std::move(other.quantity);

            // Reset the moved-from object
            other.isbn.clear();
            other.name.clear();
            other.author.clear();
            other.category.clear();
            other.price.clear();
            other.quantity.clear();

        }
        return *this;
    }

    // Destructor
    ~Book() {
    }

    // Convert to JSON
    Json::Value toJson() const {
        Json::Value json;
        json["isbn"] = isbn;
        json["name"] = name;
        json["author"] = author;
        json["category"] = category;
        json["price"] = price;
        json["quantity"] = quantity;
        return json;
    }

    // Initialize from JSON
    void fromJson(const Json::Value &json) {
        isbn = json["isbn"].asString();
        name = json["name"].asString();
        author = json["author"].asString();
        category = json["category"].asString();
        price = json["price"].asString();
        quantity = json["quantity"].asString();
    }
};