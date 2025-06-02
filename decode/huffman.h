/* huffman.h */

//
// Functions to read Huffman tree
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

// Helper function to read a Huffman tree from a JSON formatted input stream
// Same format at written by writeTreeJson in encoding portions of code
HuffmanNode* readTreeJson(std::istream& is);
