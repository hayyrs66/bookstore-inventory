#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

class HuffmanTree
{
private:
    struct Node
    {
        char ch;
        int freq;
        Node* _left, *_right;

        Node(char ch, int freq) : ch(ch), freq(freq), _left(nullptr), _right(nullptr) {}
        ~Node() { delete _left; delete _right; } // Destructor to prevent memory leaks
    };

    Node* root;

    // Compare function for priority queue
    struct CompareNodes
    {
        bool operator()(Node* l, Node* r) const
        {
            if (l->freq == r->freq)
            {
                // If frequencies are the same, order by character (lexicographically)
                if (l->ch != '\0' && r->ch != '\0')
                {
                    return l->ch > r->ch; // Lexicographical order for leaf nodes
                }
                return l->ch == '\0'; // Internal nodes (ch == '\0') should have lower priority
            }
            return l->freq > r->freq; // Otherwise, order by frequency (ascending)
        }
    };

    // Recursive function to generate Huffman codes
    void generateCodes(Node* node, string str, unordered_map<char, string>& huffmanCodes)
    {
        if (!node)
            return;

        // If this is a leaf node, it contains a character
        if (!node->_left && !node->_right)
        {
            huffmanCodes[node->ch] = str.empty() ? "0" : str; // Handle the single-character case
        }

        generateCodes(node->_left, str + "0", huffmanCodes);
        generateCodes(node->_right, str + "1", huffmanCodes);
    }

    void buildHuffmanTree(const string& data)
    {
        if (data.empty())
            return; // Handle empty input case

        unordered_map<char, int> frequency;

        // Count frequency of each character
        for (char ch : data)
        {
            frequency[ch]++;
        }

        // Create a priority queue to store live nodes of the Huffman tree
        priority_queue<Node*, vector<Node*>, CompareNodes> pq;

        // Create a leaf node for each character and add it to the priority queue
        for (const auto& pair : frequency)
        {
            pq.push(new Node(pair.first, pair.second));
        }

        // Iterate until the tree is complete
        while (pq.size() > 1)
        {
            Node* temp = pq.top();
            pq.pop();
            Node* newLeft = temp;

            temp = pq.top();
            pq.pop();
            Node* newRight = temp;

            // Create a new internal node with frequency equal to the sum of the two nodes
            Node* newNode = new Node('\0', newLeft->freq + newRight->freq);
            newNode->_left = newLeft;
            newNode->_right = newRight;

            // Add the new internal node to the priority queue
            pq.push(newNode);
        }

        // The root of the Huffman Tree is now the only node left in the queue
        root = pq.top();
        pq.pop();
    }

public:
    // Constructor to build the Huffman tree
    HuffmanTree(const string& data)
    {
        buildHuffmanTree(data);
    }

    // Destructor to prevent memory leaks
    ~HuffmanTree() { delete root; }

    // Generate Huffman codes and return the encoded string
    string encode(const string& data)
    {
        unordered_map<char, string> huffmanCodes;
        generateCodes(root, "", huffmanCodes);

        string encodedString;
        for (char ch : data)
        {
            encodedString += huffmanCodes[ch];
        }
        return encodedString;
    }

    // Get the size of the encoded string in bits
    int getEncodedSize(const string& data)
    {
        string encodedString = encode(data);
        return encodedString.size(); // Return the size in bits
    }
};