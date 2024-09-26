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
#include "ArithmeticCompressionInt.h"

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
                    // Compression for the book name using ArithmeticCompressionInt
                    ArithmeticCompressionInt arithmeticCompression(book.name);
                    std::string compressedData = arithmeticCompression.Compress(book.name);

                    // Calculate the size of the compressed data in bits
                    int arithmeticSize = compressedData.length();

                    outputBuffer << "{\"isbn\":\"" << book.isbn << "\","
                                 << "\"name\":\"" << book.name << "\","
                                 << "\"author\":\"" << book.author << "\","
                                 << "\"category\":\"" << book.category << "\","
                                 << "\"price\":\"" << book.price << "\","
                                 << "\"quantity\":\"" << book.quantity << "\","
                                 << "\"namesize\":\"" << book.name.size() * 8 << "\","
                                 << "\"namesizearithmetic\":\"" << arithmeticSize << "\"}\n";
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

        // Read line by line from the file
        while (getline(inFile, line))
        {
            std::istringstream iss(line);
            std::string operation, jsonStr;
            getline(iss, operation, ';'); // Read the operation type (INSERT, DELETE, PATCH, etc.)
            getline(iss, jsonStr, ';');   // Read the corresponding JSON for the operation

            // Remove unnecessary quotes around the JSON
            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"')
            {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            // Replace internal double quotes with single quotes
            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos)
            {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            // Parse the JSON
            Json::Value jsonData;
            std::istringstream s(jsonStr);
            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs))
            {
                continue;
            }

            // Process the corresponding operation
            if (operation == "INSERT")
            {
                Book book;
                book.fromJson(jsonData);

                inventary.insert(book); // Insert the book into the B-tree and bookMap
            }
            else if (operation == "PATCH")
            {
                inventary.update(jsonData); // Update book in the B-tree and bookMap
            }
            else if (operation == "DELETE")
            {
                inventary.remove(jsonData["isbn"].asString()); // Remove book from the B-tree and bookMap
            }
            else
            {
                std::cerr << "Unknown operation: " << operation << std::endl;
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
        int eitherCount = 0; // For cases where Huffman and Arithmetic sizes are equal

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
                    // Compression for the book name using ArithmeticCompressionInt
                    ArithmeticCompressionInt arithmeticCompression(book.name);
                    std::string compressedData = arithmeticCompression.Compress(book.name);

                    // Original size in bytes (assuming 2 bytes per character)
                    int originalSize = book.name.size() * 2;

                    // Huffman compression
                    HuffmanTree huffmanTree(book.name);
                    int huffmanSizeBits = huffmanTree.getEncodedSize(book.name);

                    // Arithmetic size in bytes
                    int arithmeticSize = (compressedData.length() + 7) / 8;

                    // Convert Huffman size to bytes for comparison
                    int huffmanSizeBytes = (huffmanSizeBits + 7) / 8;

                    // Write results with compression details
                    outFile << "{\"isbn\":\"" << book.isbn << "\","
                            << "\"name\":\"" << book.name << "\","
                            << "\"author\":\"" << book.author << "\","
                            << "\"category\":" << (book.category.empty() ? "null" : "\"" + book.category + "\"") << ","
                            << "\"price\":\"" << book.price << "\","
                            << "\"quantity\":\"" << book.quantity << "\","
                            << "\"namesize\":\"" << originalSize << "\","
                            << "\"namesizehuffman\":\"" << huffmanSizeBits << "\","
                            << "\"namesizearithmetic\":\"" << arithmeticSize << "\"}\n";

                    // Now update the counters based on the compression sizes
                    if (originalSize == huffmanSizeBytes && originalSize == arithmeticSize)
                    {
                        equalCount++;
                    }
                    else if (originalSize < huffmanSizeBytes && originalSize < arithmeticSize)
                    {
                        decompressCount++;
                    }
                    else if (huffmanSizeBytes <= arithmeticSize && huffmanSizeBytes < originalSize)
                    {
                        huffmanCount++;
                    }
                    else if (arithmeticSize < huffmanSizeBytes && arithmeticSize < originalSize)
                    {
                        arithmeticCount++;
                    }
                    else if (huffmanSizeBytes == arithmeticSize && huffmanSizeBytes < originalSize)
                    {
                        eitherCount++;
                    }
                }
            }
        }

        // Append the additional lines at the end of the output file
        outFile << "Equal: " << equalCount << "\n";
        outFile << "Decompress: " << decompressCount << "\n";
        outFile << "Huffman: " << huffmanCount << "\n";
        outFile << "Arithmetic: " << arithmeticCount << "\n";
        if (eitherCount > 0)
        {
            outFile << "Either: " << eitherCount << "\n";
        }

        // Close files
        inFile.close();
        outFile.close();
    }

    void printInventory()
    {
        inventary.printInventary();
    }
};
