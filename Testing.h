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
#include "HuffmanTree.h"
#include "ArithmeticCompression.h"

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

    void processChunk(Inventary &inventary, const std::vector<std::string> &lines, int start, int end, std::ofstream &outFile, std::mutex &fileMutex)
    {
        for (int i = start; i < end; ++i)
        {
            std::istringstream iss(lines[i]);
            std::string operation, jsonStr;
            getline(iss, operation, ';');
            getline(iss, jsonStr, ';');

            // Sanitize JSON string
            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"')
            {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }
            std::string doubleQuotes = "\"\"";
            std::string singleQuote = "\"";
            std::size_t pos = 0;
            while ((pos = jsonStr.find(doubleQuotes, pos)) != std::string::npos)
            {
                jsonStr.replace(pos, doubleQuotes.length(), singleQuote);
                pos += singleQuote.length();
            }

            // Parse JSON
            Json::Value jsonData;
            if (!parseJson(jsonStr, jsonData))
            {
                continue;
            }

            // Handle search operation
            if (operation == "SEARCH")
            {
                std::vector<Book> results = inventary.searchByName(jsonData["name"].asString());
                std::ostringstream outputBuffer;

                for (const auto &book : results)
                {
                    // Compression for the book name using Arithmetic Compression
                    ArithmeticCompression arithComp(book.name);
                    std::vector<uint8_t> compressedData = arithComp.Compress(book.name);

                    // Create a string from the compressed data
                    std::string compressedStr;
                    for (uint8_t byte : compressedData)
                    {
                        compressedStr += std::to_string(byte) + " ";
                    }

                    outputBuffer << "{\"isbn\":\"" << book.isbn << "\","
                                 << "\"name\":\"" << book.name << "\","
                                 << "\"author\":\"" << book.author << "\","
                                 << "\"category\":\"" << book.category << "\","
                                 << "\"price\":\"" << book.price << "\","
                                 << "\"quantity\":\"" << book.quantity << "\","
                                 << "\"namesize\":\"" << book.name.size() * 2 << "\","
                                 << "\"namesizearithmetic\":\"" << compressedData.size() << "\"}\n";

                }

                // Write output with locking to ensure thread safety
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

        int equalCount = 0;
        int decompressCount = 0;
        int huffmanCount = 0;
        int arithmeticCount = 0;

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
                    // **Compression for the book name using Arithmetic Compression**
                    ArithmeticCompression arithmeticCompression(book.name);
                    std::vector<uint8_t> compressedData = arithmeticCompression.Compress(book.name);

                    // Create a string from the compressed data
                    std::string compressedStr;
                    for (uint8_t byte : compressedData)
                    {
                        compressedStr += std::to_string(byte) + " ";
                    }

                    // **Original size**: 2 bytes per character
                    int originalSize = book.name.size() * 2;

                    // Huffman compression (assuming HuffmanTree is implemented)
                    HuffmanTree huffmanTree(book.name);
                    int huffmanSizeBits = huffmanTree.getEncodedSize(book.name);
                    double huffmanSizeBytes = huffmanSizeBits / 8.0; // Convert from bits to bytes

                    // **Arithmetic size** from the compressed data
                    double arithmeticSize = compressedData.size();

                    // Write results with compression details
                    outFile << "{\"isbn\":\"" << book.isbn << "\","
                            << "\"name\":\"" << book.name << "\","
                            << "\"author\":\"" << book.author << "\","
                            << "\"category\":\"" << book.category << "\","
                            << "\"price\":\"" << book.price << "\","
                            << "\"quantity\":\"" << book.quantity << "\","
                            << "\"namesize\":\"" << originalSize << "\","
                            << "\"namesizehuffman\":\"" << huffmanSizeBits << "\","
                            << "\"namesizearithmetic\":\"" << arithmeticSize << "\"}\n";

                    // **Counters update**
                    if (originalSize == huffmanSizeBytes && originalSize == arithmeticSize)
                    {
                        equalCount++;
                    }
                    else if (originalSize < huffmanSizeBytes && originalSize < arithmeticSize)
                    {
                        decompressCount++;
                    }
                    else if (huffmanSizeBytes < originalSize && huffmanSizeBytes < arithmeticSize)
                    {
                        huffmanCount++;
                    }
                    else if (arithmeticSize < originalSize && arithmeticSize < huffmanSizeBytes)
                    {
                        arithmeticCount++;
                    }
                }
            }
        }

        // Append the additional four lines at the end of the output file
        outFile << "Equal: " << equalCount << "\n";
        outFile << "Decompress: " << decompressCount << "\n";
        outFile << "Huffman: " << huffmanCount << "\n";
        outFile << "Arithmetic: " << arithmeticCount << "\n";

        // Close files
        inFile.close();
        outFile.close();
    }

    void printInventory()
    {
        inventary.printInventary();
    }
};
