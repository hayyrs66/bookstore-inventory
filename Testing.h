#pragma once
#include "Inventary.h"
#include <fstream>
#include <iostream>

class Testing {
    Inventary inventary;

public:
    Testing(int t) : inventary(t) {}

    void uploadBooks(const std::string& filename) {
        std::ifstream inFile(filename);
        std::string line;

        while (getline(inFile, line)) {
            std::istringstream iss(line);
            std::string operation;
            getline(iss, operation, ';');
            std::string jsonStr;
            getline(iss, jsonStr, ';');

            // Remove leading and trailing quotes
            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"') {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            // Replace duplicated quotes
            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos) {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            Json::CharReaderBuilder readerBuilder;
            Json::Value jsonData;
            std::string errs;
            std::istringstream s(jsonStr);

            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs)) {
                std::cerr << "Error parsing JSON: " << errs << std::endl;
                continue;
            }

            if (operation == "INSERT") {
                Book book = Book::fromJson(jsonData);
                inventary.insert(book);
            }
            else if (operation == "PATCH") {
                inventary.update(jsonData);
            }
            else if (operation == "REMOVE") {
                inventary.remove(jsonData["isbn"].asString());
            }
        }
    }


    void executeSearch(const std::string& fileToSearch, const std::string& outputFile) {
        std::ifstream inFile(fileToSearch);
        std::ofstream outFile(outputFile);
        std::string line;

        while (getline(inFile, line)) {
            std::istringstream iss(line);
            std::string operation;
            getline(iss, operation, ';');
            std::string jsonStr;
            getline(iss, jsonStr, ';');

            // Remove leading and trailing quotes
            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"') {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            // Replace duplicated quotes
            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos) {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            Json::CharReaderBuilder readerBuilder;
            Json::Value jsonData;
            std::string errs;
            std::istringstream s(jsonStr);

            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs)) {
                std::cerr << "Error parsing JSON: " << errs << std::endl;
                continue;
            }

            if (operation == "SEARCH") {
                vector<Book> results = inventary.searchByName(jsonData["name"].asString());
                for (const auto& book : results) {
                    Json::StreamWriterBuilder writer;
                    writer["indentation"] = "";
                    string jsonStr = Json::writeString(writer, book.toJson());
                    outFile << jsonStr << endl;
				}
            }
        }
    }

    void printInventory() {
        inventary.printInventary();
    }
};
