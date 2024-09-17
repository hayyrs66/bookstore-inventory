#pragma once
#include "Inventary.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>

class Testing
{
    Inventary inventary;

public:
    Testing(int t) : inventary(t) {}

    bool parseJson(const std::string &jsonStr, Json::Value &jsonData)
    {
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream s(jsonStr);
        return Json::parseFromStream(readerBuilder, s, &jsonData, &errs);
    }

    void processChunk(Inventary &inventary, const std::vector<std::string> &lines, int start, int end, std::ofstream &outFile, std::mutex &fileMutex) {
    for (int i = start; i < end; ++i) {
        std::istringstream iss(lines[i]);
        std::string operation, jsonStr;
        getline(iss, operation, ';');
        getline(iss, jsonStr, ';');

        // Sanitize JSON string
        if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"') {
            jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
        }
        std::string doubleQuotes = "\"\"";
        std::string singleQuote = "\"";
        std::size_t pos = 0;
        while ((pos = jsonStr.find(doubleQuotes, pos)) != std::string::npos) {
            jsonStr.replace(pos, doubleQuotes.length(), singleQuote);
            pos += singleQuote.length();
        }

        // Parse JSON
        Json::Value jsonData;
        if (!parseJson(jsonStr, jsonData)) {
            continue;
        }

        // Handle search operation
        if (operation == "SEARCH") {
            std::vector<Book> results = inventary.searchByName(jsonData["name"].asString());
            std::ostringstream outputBuffer;
            for (const auto& book : results) {
                std::vector<std::string> keyOrder = {"isbn", "name", "author", "category", "price", "quantity"};
                outputBuffer << inventary.sortedStr(book.toJson(), keyOrder) << std::endl;
            }

            std::lock_guard<std::mutex> lock(fileMutex);
            outFile << outputBuffer.str();
        }
    }
}


    void uploadBooks(const std::string &filename)
    {
        std::ifstream inFile(filename);
        if (!inFile.is_open())
        {
            return;
        }

        std::string line;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;

        // Leer línea por línea del archivo
        while (getline(inFile, line))
        {
            std::istringstream iss(line);
            std::string operation, jsonStr;
            getline(iss, operation, ';'); // Leer el tipo de operación (INSERT, DELETE, PATCH, etc.)
            getline(iss, jsonStr, ';');   // Leer el JSON correspondiente a la operación

            // Eliminar comillas innecesarias alrededor del JSON
            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"')
            {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            // Reemplazar comillas dobles internas con comillas simples
            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos)
            {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            // Depuración: mostrar la operación y el JSON leído

            // Parsear el JSON
            Json::Value jsonData;
            std::istringstream s(jsonStr);
            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs))
            {
                continue;
            }

            // Procesar la operación correspondiente
            if (operation == "INSERT")
            {
                Book book;
                book.fromJson(jsonData);

                inventary.insert(book); // Insertar el libro en la B-tree y el bookMap
            }
            else if (operation == "PATCH")
            {
                inventary.update(jsonData); // Actualizar libro en la B-tree y el bookMap
            }
            else if (operation == "DELETE")
            {
                inventary.remove(jsonData["isbn"].asString()); // Eliminar libro de la B-tree y el bookMap
            }
            else
            {
                std::cerr << "Operación desconocida: " << operation << std::endl;
            }
        }

        inFile.close();

    }

    void executeSearch(const std::string &fileToSearch, const std::string &outputFile)
    {
        // Open search query file
        std::ifstream inFile(fileToSearch);
        if (!inFile.is_open())
        {
            return;
        }

        // Prepare output file
        std::ofstream outFile(outputFile);
        if (!outFile.is_open())
        {
            return;
        }

        std::string line;
        while (getline(inFile, line))
        {
            std::istringstream iss(line);
            std::string operation, jsonStr;
            getline(iss, operation, ';');
            getline(iss, jsonStr, ';');

            // Remove leading and trailing quotes
            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"')
            {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            // Replace duplicated quotes
            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos)
            {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            // Parse JSON
            Json::CharReaderBuilder readerBuilder;
            Json::Value jsonData;
            std::string errs;
            std::istringstream s(jsonStr);
            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs))
            {
                continue;
            }

            // Process search operation
            if (operation == "SEARCH" && jsonData.isMember("name"))
            {
                // Search books by name
                std::vector<Book> results = inventary.searchByName(jsonData["name"].asString());

                // Write results in the same order as the search query
                for (const auto &book : results)
                {
                    vector<string> keyOrder = {"isbn", "name", "author", "category", "price", "quantity"};
                    string orderedJsonStr = inventary.sortedStr(book.toJson(), keyOrder);
                    outFile << orderedJsonStr << endl;
                }
            }
            // std::cerr << "Searching for book name: " << jsonData["name"].asString() << std::endl;
        }

        inFile.close();
        outFile.close();
    }

    void printInventory()
    {
        inventary.printInventary();
    }
};