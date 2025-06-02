/* huffman.h */

//
// Functions to create and manipulate a Huffman tree
//

#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>

/// <summary>
/// A HuffmanNode represents a node in the Huffman tree.
/// It can be a leaf node containing a character and its frequency,
/// or an internal node that combines two child nodes.
/// The way an internal node is detected is by checking if its character is '\0'.
/// </summary>

// Constructor for a leaf node
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    // leaf
    HuffmanNode(char character, int frequency);

    // internal node
    HuffmanNode(HuffmanNode* l, HuffmanNode* r);
};

// Compare two HuffmanNode pointers by frequency (for min‐heap priority_queue)
struct NodeCompare {
    bool operator()(HuffmanNode* lhs, HuffmanNode* rhs) const;
};

// Build a Huffman tree from a frequency map
HuffmanNode* buildHuffmanTree(std::unordered_map<char, int>& freqMap);

// Build a map of characters to their corresponding Huffman codes
void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codes);

// Write the Huffman tree to a JSON format
void writeTreeJson(HuffmanNode* root, std::ostream& os);
