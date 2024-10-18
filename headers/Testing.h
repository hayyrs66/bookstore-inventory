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
#include "Des.h"

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

            Json::Value jsonData;
            if (!parseJson(jsonStr, jsonData))
            {
                continue;
            }

            if (operation == "SEARCH")
            {
                std::vector<Book> results = inventary.searchByName(jsonData["name"].asString());
                std::ostringstream outputBuffer;

                for (const auto &book : results)
                {
                    ArithmeticCompressionInt arithmeticCompression(book.name);
                    std::string compressedData = arithmeticCompression.Compress(book.name);

                    int arithmeticSize = compressedData.length();

                    outputBuffer << "{\"isbn\":\"" << book.isbn << "\","
                                 << "\"name\":\"" << book.name << "\","
                                 << "\"author\":\"" << book.author << "\","
                                 << "\"category\":\"" << book.category << "\","
                                 << "\"price\":\"" << book.price << "\","
                                 << "\"quantity\":\"" << book.quantity << "\"}/n";
                    //  << "\"namesize\":\"" << book.name.size() * 8 << "\","
                    //  << "\"namesizearithmetic\":\"" << arithmeticSize << "\"}\n";
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

        while (getline(inFile, line))
        {
            std::istringstream iss(line);
            std::string operation, jsonStr;
            getline(iss, operation, ';');
            getline(iss, jsonStr, ';');

            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"')
            {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos)
            {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            Json::Value jsonData;
            std::istringstream s(jsonStr);
            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs))
            {
                continue;
            }

            if (operation == "INSERT")
            {
                Book book;
                book.fromJson(jsonData);
                inventary.insert(book);
            }
            else if (operation == "PATCH")
            {
                inventary.update(jsonData);
            }
            else if (operation == "DELETE")
            {
                inventary.remove(jsonData["isbn"].asString());
            }
            else
            {
                std::cerr << "Unknown operation: " << operation << std::endl;
            }
        }

        inFile.close();
    }

    void executeSearch(const std::string &fileToSearch, const std::string &outputFile, const std::string &encryptedOutputFile, const std::string &key)
    {
        std::ifstream inFile(fileToSearch);
        if (!inFile.is_open())
        {
            return;
        }

        std::ofstream outFile(outputFile);
        if (!outFile.is_open())
        {
            return;
        }

        int equalCount = 0;
        int decompressCount = 0;
        int huffmanCount = 0;
        int arithmeticCount = 0;
        int eitherCount = 0;

        std::string line;
        while (getline(inFile, line))
        {
            std::istringstream iss(line);
            std::string operation, jsonStr;
            getline(iss, operation, ';');
            getline(iss, jsonStr, ';');

            if (!jsonStr.empty() && jsonStr.front() == '"' && jsonStr.back() == '"')
            {
                jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
            }

            size_t pos = 0;
            while ((pos = jsonStr.find("\"\"", pos)) != std::string::npos)
            {
                jsonStr.replace(pos, 2, "\"");
                pos += 1;
            }

            Json::CharReaderBuilder readerBuilder;
            Json::Value jsonData;
            std::string errs;
            std::istringstream s(jsonStr);
            if (!Json::parseFromStream(readerBuilder, s, &jsonData, &errs))
            {
                continue;
            }

            if (operation == "SEARCH" && jsonData.isMember("name"))
            {
                std::vector<Book> results = inventary.searchByName(jsonData["name"].asString());
                for (const auto &book : results)
                {
                    ArithmeticCompressionInt arithmeticCompression(book.name);
                    std::string compressedData = arithmeticCompression.Compress(book.name);

                    int originalSize = book.name.size() * 2;
                    HuffmanTree huffmanTree(book.name);
                    int huffmanSizeBits = huffmanTree.getEncodedSize(book.name);

                    int arithmeticSize = (compressedData.length() + 7) / 8;
                    int huffmanSizeBytes = (huffmanSizeBits + 7) / 8;

                    outFile << "{\"isbn\":\"" << book.isbn << "\","
                            << "\"name\":\"" << book.name << "\","
                            << "\"author\":\"" << book.author << "\","
                            << "\"category\":" << (book.category.empty() ? "null" : "\"" + book.category + "\"") << ","
                            << "\"price\":\"" << book.price << "\","
                            << "\"quantity\":\"" << book.quantity << "\"}\n";
                    // << "\"namesize\":\"" << originalSize << "\","
                    // << "\"namesizehuffman\":\"" << huffmanSizeBits << "\","
                    // << "\"namesizearithmetic\":\"" << arithmeticSize << "\"}\n";

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
        // outFile << "Equal: " << equalCount << "\n";
        // outFile << "Decompress: " << decompressCount << "\n";
        // outFile << "Huffman: " << huffmanCount << "\n";
        // outFile << "Arithmetic: " << arithmeticCount << "\n";
        // if (eitherCount > 0)
        // {
        //     outFile << "Either: " << eitherCount << "\n";
        // }

        inFile.close();
        outFile.close();

        Des des;
        des.encryptFile(outputFile, encryptedOutputFile, key);
    }

    void printInventory()
    {
        inventary.printInventary();
    }
};
