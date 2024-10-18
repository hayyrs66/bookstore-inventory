#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <queue>
#include <cmath>
#include <iomanip>
#include <cctype>
#include <cstdint>

using namespace std;

class ArithmeticCompressionInt {
private:
    unordered_map<char, pair<uint16_t, uint16_t>> probabilities;
    string source;

    static const uint16_t NUMBER_BITS = 16;
    static const uint16_t default_low = 0;
    static const uint16_t default_high = 0xFFFF;
    static const uint16_t MSD = 0x8000;
    static const uint16_t SSD = 0x4000;
    static const uint16_t TOO = 0x3fff;

    uint16_t scale;

public:
    ArithmeticCompressionInt(const string& source) : source(source), scale(0) {
        CalculateProbabilities();
    }

    ArithmeticCompressionInt(const unordered_map<char, pair<uint16_t, uint16_t>>& probabilities, uint16_t scale)
        : probabilities(probabilities), scale(scale) {}

    void CalculateProbabilities() {
        unordered_map<char, uint16_t> frequencies;

        for (char symbol : source) {
            frequencies[symbol]++;
        }

        vector<pair<char, uint16_t>> sortedFreqs(frequencies.begin(), frequencies.end());
        sort(sortedFreqs.begin(), sortedFreqs.end(), [](const auto& a, const auto& b) {
            return a.second < b.second || (a.second == b.second && a.first < b.first);
            });

        scale = static_cast<uint16_t>(source.length());
        uint16_t low = 0;

        for (const auto& symbol : sortedFreqs) {
            uint16_t high = low + symbol.second;
            probabilities[symbol.first] = { low, high };
            low = high;
        }
    }

    string Compress(const string& input) {
        string output_stream;

        uint16_t low = default_low;
        uint16_t high = default_high;
        uint64_t underflow_bits = 0;

        for (char symbol : input) {
            uint64_t range = static_cast<uint64_t>(high - low) + 1;
            high = static_cast<uint16_t>(low + (range * probabilities[symbol].second) / scale - 1);
            low = static_cast<uint16_t>(low + (range * probabilities[symbol].first) / scale);

            while (true) {
                if ((high & MSD) == (low & MSD)) {
                    bool bit = (high & MSD) != 0;
                    output_stream.append(bit ? "1" : "0");
                    while (underflow_bits > 0) {
                        bit = (high & MSD) == 0;
                        output_stream.append(bit ? "1" : "0");
                        underflow_bits--;
                    }
                }
                else if ((low & SSD) != 0 && (high & SSD) == 0) {
                    underflow_bits++;
                    low &= TOO;
                    high |= SSD;
                }
                else {
                    break;
                }
                low <<= 1;
                high <<= 1;
                high |= 1;
            }
        }

        bool finalBit = (low & SSD) != 0;
        output_stream.append(finalBit ? "1" : "0");
        underflow_bits++;
        while (underflow_bits > 0) {
            bool bit = (low & SSD) == 0;
            output_stream.append(bit ? "1" : "0");
            underflow_bits--;
        }

        // Pad the output to make it a multiple of 8 bits
        if (output_stream.length() % 8 != 0) {
            output_stream.append(8 - output_stream.length() % 8, '0');
        }

        return output_stream;
    }

    string Decompress(string input, int size) {
        char c;
        uint16_t scaledValue;
        stringstream retval;
        uint16_t code = 0;
        uint16_t low = default_low;
        uint16_t high = default_high;

        string input_buffer = input;
        for (int i = 0; i < NUMBER_BITS; i++) {
            code <<= 1;
            code |= input_buffer.at(0) == '1' ? 1 : 0;
            input_buffer.erase(0, 1);
        }

        for (int i = 0; i < size; i++) {
            uint64_t range = static_cast<uint64_t>(high - low) + 1;
            scaledValue = static_cast<uint16_t>((static_cast<uint64_t>(code - low + 1) * scale - 1) / range);

            c = '\0';
            for (const auto& symbol : probabilities) {
                if (scaledValue >= symbol.second.first && scaledValue < symbol.second.second) {
                    c = symbol.first;
                    break;
                }
            }

            if (c == '\0') throw runtime_error("Decoding Error");

            retval << c;

            range = static_cast<uint64_t>(high - low) + 1;
            high = static_cast<uint16_t>(low + (range * probabilities[c].second) / scale - 1);
            low = static_cast<uint16_t>(low + (range * probabilities[c].first) / scale);

            while (true) {  
                if ((high & MSD) == (low & MSD)) {
                    // Shift out the most significant bit
                }
                else if ((low & SSD) == SSD && (high & SSD) == 0) {
                    code ^= SSD;
                    low &= TOO;
                    high |= SSD;
                }
                else {
                    break;
                }
                low <<= 1;
                high <<= 1;
                high |= 1;
                code <<= 1;
                if (input_buffer.length() == 0) break;
                code |= input_buffer.at(0) == '1' ? 1 : 0;
                input_buffer.erase(0, 1);
            }
        }
        return retval.str();
    }
};
