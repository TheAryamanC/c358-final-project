/* huffman.cpp */

//
// Implementation of functions to read Huffman tree
//

#include <cctype>
#include <queue>
#include <stdexcept>
#include <sstream>
#include <vector>

#include "huffman.h"

using namespace std;

// leaf
HuffmanNode::HuffmanNode(char character, int frequency)
    : ch(character), freq(frequency), left(nullptr), right(nullptr) {}

// internal node
HuffmanNode::HuffmanNode(HuffmanNode* l, HuffmanNode* r)
    : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}

/// Helpers for reading JSON formatted Huffman tree
/// Credit to: https://dev.to/uponthesky/c-making-a-simple-json-parser-from-scratch-250g for help
// Skips whitespace characters
void skipWhitespace(istream& is) 
{
    while (is.good() && isspace(is.peek())) 
    {
        is.get(); // skip whitespace
    }
}
// Parse string (key)
string parseString(istream& is) 
{
    skipWhitespace(is);
    if (is.get() != '"') 
    {
        throw runtime_error("Expected quotation mark!");
    }

    string result;
    while (is.good()) 
    {
        char c = is.get();
        if (c == '\\') 
        {
            char esc = is.get();
            switch (esc) {
                case '"': result.push_back('"'); break;
                case '\\': result.push_back('\\'); break;
                case '/': result.push_back('/'); break;
                case 'b': result.push_back('\b'); break;
                case 'f': result.push_back('\f'); break;
                case 'n': result.push_back('\n'); break;
                case 'r': result.push_back('\r'); break;
                case 't': result.push_back('\t'); break;
                default: throw runtime_error("Unknown sequence!");
            }
        } // escape sequences (newline, tab, etc.)
        else if (c == '"') 
        {
            return result;
        } // end of string
        else 
        {
            result.push_back(c);
        } // regular character
    }
    
    // something went wrong
    throw runtime_error("Something else went wrong!");
}
// Parse integer (value)
static int parseInt(istream& is) 
{
    skipWhitespace(is);
    if (!isdigit(is.peek())) 
    {
        throw runtime_error("Expected digit!");
    }

    int value = 0;
    while (is.good() && isdigit(is.peek())) 
    {
        // if we add a digit, we need to multiply the current value by 10 then add the new digit
        value = value * 10 + (is.get() - '0');
    }
    return value;
}
// Build HuffmanNode
HuffmanNode* parseNode(istream& is) 
{
    // first character is '{'
    skipWhitespace(is);
    if (is.peek() != '{') 
    {
        throw runtime_error("Need '{' at beginning!");
    }
    is.get();
    skipWhitespace(is);

    // then key
    string key = parseString(is);
    skipWhitespace(is);

    // key/value pair is separated by ':'
    if (is.get() != ':') 
    {
        throw runtime_error("Key/value pair must be separated by ':'");
    }
    skipWhitespace(is);

    HuffmanNode* node = nullptr;
    // then depending on key, we have leaf or internal node
    if (key == "ch") 
    {
        // get character (written as integer)
        int chInt = parseInt(is);
        skipWhitespace(is);
        if (is.get() != ',') 
        {
            throw runtime_error("Character/frequency pair must be separated by ','");
        }
        skipWhitespace(is);

        // get frequency
        string key2 = parseString(is);
        if (key2 != "freq") 
        {
            throw runtime_error("Second key should be freq!");
        }
        skipWhitespace(is);
        if (is.get() != ':') 
        {
            throw runtime_error("Key/value pair must be separated by ':'");
        }
        int freqInt = parseInt(is);
        skipWhitespace(is);
        if (is.get() != '}') 
        {
            throw runtime_error("Need '}' at end of leaf!");
        }

        // create leaf node
        node = new HuffmanNode(static_cast<char>(chInt), freqInt);
        return node;
    } // leaf
    else if (key == "freq") 
    {
        // get frequency
        int freqInt = parseInt(is);
        skipWhitespace(is);
        if (is.get() != ',') 
        {
            throw runtime_error("Frequency/left/right triple must be separated by ','");
        }
        skipWhitespace(is);

        // get left subtree
        string key2 = parseString(is);
        if (key2 != "left") 
        {
            throw runtime_error("Key should be left!");
        }
        skipWhitespace(is);
        if (is.get() != ':') 
        {
            throw runtime_error("Key/value pair must be separated by ':'");
        }
        // parse left subtree recursively
        HuffmanNode* leftChild = parseNode(is);

        skipWhitespace(is);
        if (is.get() != ',') 
        {
            throw runtime_error("Frequency/left/right triple must be separated by ','");
        }
        skipWhitespace(is);

        // get right subtree
        string key3 = parseString(is);
        if (key3 != "right") 
        {
            throw runtime_error("Key should be right!");
        }
        skipWhitespace(is);
        if (is.get() != ':') 
        {
            throw runtime_error("Key/value pair must be separated by ':'");
        }
        // parse right subtree recursively
        HuffmanNode* rightChild = parseNode(is);

        // last character should be '}'
        skipWhitespace(is);
        if (is.get() != '}') 
        {
            throw runtime_error("Need '}' at end of node!");
        }

        // create internal node
        node = new HuffmanNode(leftChild, rightChild);
        node->freq = freqInt;
        node->ch = '\0';
        return node;
    } // internal node
    else 
    {
        throw runtime_error("Something went wrong!");
    } // something went wrong
}
// Fully read tree
HuffmanNode* readTreeJson(istream& is) 
{
    return parseNode(is);
}
