#pragma once
#include "BTree.h"
#include <json/json.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

class Inventary
{
    BTree tree;
    unordered_map<string, Book> bookMap;

public:
    Inventary(int t) : tree(t) {}

    void exportBookMapToJson(const std::string &filename)
    {
        std::ofstream outFile(filename);
        if (!outFile.is_open())
        {
            return;
        }

        for (const auto &pair : bookMap)
        {
            Json::Value bookJson = pair.second.toJson();

            Json::StreamWriterBuilder writer;
            writer["indentation"] = "";
            writer["dropNullPlaceholders"] = true;

            std::string jsonLine = Json::writeString(writer, bookJson);
            outFile << jsonLine << std::endl;
        }

        outFile.close();
    }

    void insert(const Book &book)
    {
        tree.insert(book);
        bookMap[book.name] = book;
    }

    void remove(const std::string &isbn)
    {
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            tree.remove(isbn);

            auto it = std::find_if(bookMap.begin(), bookMap.end(), [&](const auto &pair)
            { return pair.second.isbn == isbn; });

            if (it != bookMap.end())
            {
                bookMap.erase(it);
            }
        }
    }

    void update(const Json::Value &bookData)
    {
        std::string isbn = bookData["isbn"].asString();
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            for (auto &book : node->keys)
            {
                if (book.isbn == isbn)
                {
                    std::string oldName = book.name;

                    if (bookData.isMember("name"))
                    {
                        std::string newName = bookData["name"].asString();
                        book.name = newName;
                    }
                    if (bookData.isMember("author"))
                    {
                        std::string newAuthor = bookData["author"].asString();
                        book.author = newAuthor;
                    }
                    if (bookData.isMember("category"))
                    {
                        std::string newCategory = bookData["category"].asString();
                        book.category = newCategory;
                    }
                    if (bookData.isMember("price"))
                    {
                        std::string newPrice = bookData["price"].asString();
                        book.price = newPrice;
                    }
                    if (bookData.isMember("quantity"))
                    {
                        std::string newQuantity = bookData["quantity"].asString();
                        book.quantity = newQuantity;
                    }

                    if (oldName != book.name)
                    {
                        auto it = bookMap.find(oldName);
                        if (it != bookMap.end())
                        {
                            bookMap.erase(it);
                        }
                    }
                    bookMap[book.name] = book;
                    break;
                }
            }
        }
        // else
        // {
        //     std::cerr << "Error: Book with ISBN " << isbn << " not found in the B-tree for PATCH." << std::endl;
        // }
    }

    vector<Book> searchByName(const std::string &name)
    {
        std::vector<Book> results;
        auto it = bookMap.find(name);

        if (it != bookMap.end())
        {
            results.push_back(it->second);
        }
        return results;
    }

    void searchRecursive(BTreeNode *node, const string &name, vector<Book> &results)
    {
        if (!node)
            return;

        for (const auto &book : node->keys)
        {
            if (book.name == name)
            {
                results.push_back(book);
            }
        }

        if (!node->leaf)
        {
            for (auto &child : node->children)
            {
                searchRecursive(child.get(), name, results);
            }
        }
    }

    // Optionally: a method to find a book by ISBN
    Book *findBookByISBN(const std::string &isbn)
    {
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            for (auto &book : node->keys)
            {
                if (book.isbn == isbn)
                {
                    return &book;
                }
            }
        }
        return nullptr;
    }

    std::string sortedStr(const Json::Value &value, const std::vector<std::string> &sortKeys) const;

    void printInventary()
    {
        tree.printTree();
    }
};
