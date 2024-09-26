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
    unordered_map<string, Book> isbnToBook;
    unordered_multimap<string, string> nameToIsbn;

public:
    Inventary(int t) : tree(t) {}

    void exportBookMapToJson(const std::string &filename)
    {
        std::ofstream outFile(filename);
        if (!outFile.is_open())
        {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "";
        writerBuilder["dropNullPlaceholders"] = true;
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());

        for (const auto &pair : isbnToBook)
        {
            const Book &book = pair.second;
            Json::Value bookJson = book.toJson();

            std::ostringstream oss;
            writer->write(bookJson, &oss);
            outFile << oss.str() << std::endl;
        }

        outFile.close();
    }

    void insert(const Book &book)
    {
        tree.insert(book);
        isbnToBook[book.isbn] = book;
        nameToIsbn.emplace(book.name, book.isbn);
    }

    void remove(const std::string &isbn)
    {
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            tree.remove(isbn);

            auto bookIt = isbnToBook.find(isbn);
            if (bookIt != isbnToBook.end())
            {
                std::string name = bookIt->second.name;
                isbnToBook.erase(bookIt);

                auto range = nameToIsbn.equal_range(name);
                for (auto it = range.first; it != range.second; ++it)
                {
                    if (it->second == isbn)
                    {
                        nameToIsbn.erase(it);
                        break;
                    }
                }
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
                        book.name = bookData["name"].asString();
                    }
                    if (bookData.isMember("author"))
                    {
                        book.author = bookData["author"].asString();
                    }
                    if (bookData.isMember("category"))
                    {
                        book.category = bookData["category"].asString();
                    }
                    if (bookData.isMember("price"))
                    {
                        book.price = bookData["price"].asString();
                    }
                    if (bookData.isMember("quantity"))
                    {
                        book.quantity = bookData["quantity"].asString();
                    }

                    isbnToBook[isbn] = book;

                    if (oldName != book.name)
                    {
                        auto range = nameToIsbn.equal_range(oldName);
                        for (auto it = range.first; it != range.second; ++it)
                        {
                            if (it->second == isbn)
                            {
                                nameToIsbn.erase(it);
                                break;
                            }
                        }
                        nameToIsbn.emplace(book.name, isbn);
                    }
                    break;
                }
            }
        }
    }

    vector<Book> searchByName(const std::string &name)
    {
        std::vector<Book> results;
        auto range = nameToIsbn.equal_range(name);
        for (auto it = range.first; it != range.second; ++it)
        {
            const std::string &isbn = it->second;
            auto bookIt = isbnToBook.find(isbn);
            if (bookIt != isbnToBook.end())
            {
                results.push_back(bookIt->second);
            }
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
