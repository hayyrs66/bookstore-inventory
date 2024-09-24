#include <unordered_map>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

class ArithmeticCompression {
private:
    std::unordered_map<char, double> probabilities;

    // Compare function to sort probabilities by value first, then lexicographically
    static bool comparePairs(const std::pair<char, double>& a, const std::pair<char, double>& b) {
        if (a.second == b.second)
            return a.first < b.first;
        return a.second < b.second;
    }

    void calculateProbabilities(const std::string& data) {
        std::unordered_map<char, int> frequency;
        for (char ch : data) {
            frequency[ch]++;
        }
        int total = data.size();
        for (auto& pair : frequency) {
            probabilities[pair.first] = static_cast<double>(pair.second) / total;
            // Debug message to print calculated probabilities
            std::cerr << "[DEBUG] Character: '" << pair.first 
                      << "' Frequency: " << pair.second 
                      << " Total: " << total 
                      << " => Probability: " << probabilities[pair.first] << "\n";
        }
    }

public:
    ArithmeticCompression(const std::string& data) {
        calculateProbabilities(data);
    }

    // Get the compressed size of the data using arithmetic coding (in bytes)
    double getCompressedSize(const std::string& data) {
        double compressedBits = 0.0;

        // Calculate the compressed size in bits
        for (char ch : data) {
            // If character is missing in probabilities (edge case), handle it
            if (probabilities.find(ch) == probabilities.end()) {
                std::cerr << "[DEBUG][ERROR] Character: '" << ch << "' not found in probabilities!\n";
                continue;
            }

            double probability = probabilities[ch];
            double logValue = -log2(probability); // Bits required to encode the character
            compressedBits += logValue;

            // Debug message to print each character's contribution to the compressed size
            std::cerr << "[DEBUG] Character: '" << ch 
                      << "', Probability: " << probability 
                      << ", -log2(Probability): " << logValue 
                      << ", Accumulated Bits: " << compressedBits << "\n";
        }

        // Convert bits to bytes (8 bits per byte) and round up
        double compressedSizeInBytes = ceil(compressedBits / 8.0);

        // Debug message to print the final compressed size in bytes
        std::cerr << "[DEBUG] Final compressed size in bytes: " << compressedSizeInBytes << "\n";

        return compressedSizeInBytes;
    }
};
