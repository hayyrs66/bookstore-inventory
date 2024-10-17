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
        Node *_left, *_right;

        Node(char ch, int freq) : ch(ch), freq(freq), _left(nullptr), _right(nullptr) {}
        ~Node()
        {
            delete _left;
            delete _right;
        }
    };

    Node *root;

    struct CompareNodes
    {
        bool operator()(Node *l, Node *r) const
        {
            if (l->freq == r->freq)
                return l->ch > r->ch;
            {
                // if (l->ch != '\0' && r->ch != '\0')
                // {
                // }
                // return l->ch == '\0';
            }
            return l->freq > r->freq;
        }
    };

    void generateCodes(Node *node, string str, unordered_map<char, string> &huffmanCodes)
    {
        if (!node)
            return;

        if (!node->_left && !node->_right)
        {
            huffmanCodes[node->ch] = str.empty() ? "0" : str;
        }

        generateCodes(node->_left, str + "0", huffmanCodes);
        generateCodes(node->_right, str + "1", huffmanCodes);
    }

    void buildHuffmanTree(const string &data)
    {
        if (data.empty())
            return;

        unordered_map<char, int> frequency;
        for (char ch : data)
        {
            frequency[ch]++;
        }

        priority_queue<Node *, vector<Node *>, CompareNodes> pq;

        for (const auto &pair : frequency)
        {
            pq.push(new Node(pair.first, pair.second));
        }

        while (pq.size() > 1)
        {
            Node *temp = pq.top();
            pq.pop();
            Node *newLeft = temp;

            temp = pq.top();
            pq.pop();
            Node *newRight = temp;

            Node *newNode = new Node('\0', newLeft->freq + newRight->freq);
            newNode->_left = newLeft;
            newNode->_right = newRight;
            pq.push(newNode);
        }

        root = pq.top();
        pq.pop();
    }

public:
    HuffmanTree(const string &data)
    {
        buildHuffmanTree(data);
    }

    ~HuffmanTree() { delete root; }

    string encode(const string &data)
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

    int getEncodedSize(const string &data)
    {
        string encodedString = encode(data);
        return encodedString.size();
    }
};