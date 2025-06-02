/* huffman.cpp */

//
// Implementation of functions to create and manipulate a Huffman tree
//

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <queue>
#include <vector>

#include "huffman.h"

// leaf
HuffmanNode::HuffmanNode(char character, int frequency)
    : ch(character), freq(frequency), left(nullptr), right(nullptr) {}

// internal node
HuffmanNode::HuffmanNode(HuffmanNode* l, HuffmanNode* r)
    : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}

// smallest frequency first comparison for priority queue
bool NodeCompare::operator()(HuffmanNode* lhs, HuffmanNode* rhs) const 
{
    return lhs->freq > rhs->freq;
}

// build a Huffman tree
// Credit to Prof Hummel's 211 projects 5-8 for idea of using a priority queue
HuffmanNode* buildHuffmanTree(std::unordered_map<char, int>& freqMap) 
{
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeCompare> pq;

    // leaves for each character
    for (auto const& pair : freqMap) 
    {
        HuffmanNode* node = new HuffmanNode(pair.first, pair.second);
        pq.push(node);
    }

    // build tree by recursively combining the two smallest nodes
    while (pq.size() > 1) 
    {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* internal = new HuffmanNode(left, right);
        pq.push(internal);
    }

    return pq.top();
}

// generate Huffman codes for each character
void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codes) 
{
    // at a leaf, assign code
    if (!root->left && !root->right) 
    {
        codes[root->ch] = prefix;
        return;
    }

    // left = 0, right = 1
    generateCodes(root->left, prefix + "0", codes);
    generateCodes(root->right, prefix + "1", codes);
}

// Build HuffmanNode into JSON format
// Credit to https://marc.helbling.fr/writing-json-c/
static void writeNodeJson(HuffmanNode* node, std::ostream& os) 
{
    // leaf
    if (!node->left && !node->right) 
    {
        // format is: {"ch":<int>,"freq":<int>}
        os << "{\"ch\":" << static_cast<int>(static_cast<unsigned char>(node->ch)) << ",\"freq\":" << node->freq << "}";
    } 
    // internal node
    else 
    {
        // format is: {"freq":<int>,"left":<...>,"right":<...>}
        os << "{\"freq\":" << node->freq << ",\"left\":";
        writeNodeJson(node->left, os); // recursively write left subtree
        os << ",\"right\":";
        writeNodeJson(node->right, os); // recursively write right subtree
        os << "}";
    }
}
// Fully write out tree
void writeTreeJson(HuffmanNode* root, std::ostream& os) 
{
    writeNodeJson(root, os);
}