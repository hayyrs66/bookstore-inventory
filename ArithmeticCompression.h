#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <sstream>
#include <algorithm>

class BitWriter {
public:
    BitWriter() : buffer(0), bitCount(0) {}

    void WriteBit(bool bit) {
        buffer <<= 1;
        if (bit) buffer |= 1;
        bitCount++;

        if (bitCount == 8) {
            stream.push_back(buffer);
            bitCount = 0;
            buffer = 0;
        }
    }

    std::vector<uint8_t> Flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            stream.push_back(buffer);
        }
        return stream;
    }

    static std::string GetBinaryStringFromMemoryStream(const std::vector<uint8_t>& stream) {
        std::stringstream binaryStream;
        for (const auto& byte : stream) {
            binaryStream << std::bitset<8>(byte);
        }
        std::string binaryStr = binaryStream.str();
        
        // Format the binary string by grouping every 4 bits
        std::stringstream formattedStream;
        for (size_t i = 0; i < binaryStr.size(); i += 4) {
            if (i > 0) formattedStream << '-';
            formattedStream << binaryStr.substr(i, 4);
        }

        return formattedStream.str();
    }

private:
    std::vector<uint8_t> stream;
    uint8_t buffer;
    int bitCount;
};

class ArithmeticCompression {
public:
    ArithmeticCompression(const std::string& source)
        : source(source), overflowBits(0), low(defaultLow), high(defaultHigh) {
        CalculateProbabilities();
    }

    ArithmeticCompression(const std::map<char, std::pair<uint16_t, uint16_t>>& probabilities, uint16_t scale)
        : probabilities(probabilities), scale(scale), low(defaultLow), high(defaultHigh), overflowBits(0) {}

    std::vector<uint8_t> Compress(const std::string& input) {
        low = defaultLow;
        high = defaultHigh;
        overflowBits = 0;

        int64_t range;
        BitWriter bitWriter;

        for (const char& symbol : input) {
            range = (int64_t)(high - low) + 1;
            high = low + range * probabilities[symbol].second / scale - 1;
            low = low + range * probabilities[symbol].first / scale;

            while (true) {
                if ((high & MSD) == (low & MSD)) {
                    bitWriter.WriteBit(high & MSD);
                    while (overflowBits > 0) {
                        bitWriter.WriteBit(!(high & MSD));
                        overflowBits--;
                    }
                } else if ((low & SSD) != 0 && (high & SSD) == 0) {
                    overflowBits++;
                    low &= 0x3fff;
                    high |= 0x4000;
                } else {
                    break;
                }
                low <<= 1;
                high = (high << 1) | 1;
            }
        }

        bitWriter.WriteBit(low & 0x4000);
        overflowBits++;
        while (overflowBits-- > 0) {
            bitWriter.WriteBit(!(low & 0x4000));
        }

        return bitWriter.Flush();
    }

private:
    void CalculateProbabilities() {
        std::map<char, uint16_t> frequencies;

        for (const char& symbol : source) {
            frequencies[symbol]++;
        }

        scale = source.length();

        uint16_t low = 0;
        for (const auto& symbol : frequencies) {
            uint16_t high = low + symbol.second;
            probabilities[symbol.first] = {low, high};
            low = high;
        }
    }

    const std::string source;
    std::map<char, std::pair<uint16_t, uint16_t>> probabilities;
    uint64_t overflowBits;

    static const uint16_t defaultLow = 0;
    static const uint16_t defaultHigh = 0xffff;

    static const uint16_t MSD = 0x8000;  // Most significant bit
    static const uint16_t SSD = 0x4000;  // Second significant bit

    uint16_t scale;
    uint16_t low;
    uint16_t high;
};