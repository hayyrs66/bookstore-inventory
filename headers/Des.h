#pragma once
#define OPENSSL_SUPPRESS_DEPRECATED
#include <openssl/des.h>
#include <openssl/rand.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

class Des
{
public:
    std::string encryptDES(const std::string &plaintext, const std::string &key)
    {
        DES_cblock keyEncrypt;
        DES_key_schedule schedule;

        memset(&keyEncrypt, 0, sizeof(DES_cblock));
        size_t keyLen = key.size() > 8 ? 8 : key.size();
        memcpy(keyEncrypt, key.c_str(), keyLen);
        // Fix parity bits
        DES_set_odd_parity(&keyEncrypt);
        int err = DES_set_key_checked(&keyEncrypt, &schedule);
        if (err != 0)
        {
            std::cerr << "Key error: " << err << std::endl;
            return "";
        }

        std::string paddedText = plaintext;
        size_t padValue = 8 - (plaintext.size() % 8);
        paddedText.append(padValue, padValue);

        std::string ciphertext(paddedText.size(), 0);
        for (size_t i = 0; i < paddedText.size(); i += 8)
        {
            DES_ecb_encrypt((const_DES_cblock *)(paddedText.data() + i),
            (DES_cblock *)(ciphertext.data() + i), &schedule, DES_ENCRYPT);
        }

        return ciphertext;
    }
    void encryptFile(const std::string &inputFilePath, const std::string &outputFilePath, const std::string &key)
    {
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        std::ofstream outputFile(outputFilePath, std::ios::binary);

        std::string inputData((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());

        std::string encryptedData = encryptDES(inputData, key);
        outputFile.write(encryptedData.c_str(), encryptedData.size());

        inputFile.close();
        outputFile.close();
    }
};