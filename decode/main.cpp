/* main.cpp */

//
// Performs the decoding of a Huffman‐encoded binary file
// 
// Aryaman C
//

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "huffman.h"

using namespace std;
using namespace std::chrono;

//
// Reads the arguments from the command line
//
int readArgs(int argc, char* argv[], char*& tree, char*& binaryFile)
{
    if (argc != 3) 
    {
        cout << endl;
        cout << "Usage: " << argv[0] << " <tree.json> <encoded.bin> <output.txt>" << endl;;
        cout << endl;
        return 1;
    }

    tree = argv[1];
    binaryFile = argv[2];
    return 0;
}

//
// Reads the Huffman tree from a JSON file
// Credit to the answer in https://stackoverflow.com/questions/32205981/reading-json-files-in-c
//
int readTree(char* treeFile, HuffmanNode*& root)
{
    // open file
    ifstream jsonIn(treeFile, ifstream::binary);
    if (!jsonIn) 
    {
        cout << endl;
        cout << "Error: Cannot open tree JSON file!" << endl;
        cout << endl;
        return 1;
    }

    // parse tree
    try
    {
        root = readTreeJson(jsonIn);
        jsonIn.close();
    }
    catch (const std::exception& e)
    {
        cout << endl;
        cout << "Error parsing JSON file!" << endl;
        cout << endl;
        return 1;
    }

    return 0;
}

//
// Reads the binary file
//
int readBinaryFile(char* binaryFile, uint64_t& totalBits, vector<unsigned char>& byteBuffer, string& bitString)
{
    // open file
    ifstream binaryIn(binaryFile, ifstream::binary);
    if (!binaryIn)
    {
        cout << endl;
        cout << "Error: Cannot open binary file!" << endl;
        cout << endl;
        return 1;
    }

    // binary files begin with a 64-bit integer indicating the total number of bits
    binaryIn.read(reinterpret_cast<char*>(&totalBits), sizeof(totalBits));

    // remaining data is encoded bits
    // Credit to the question in https://stackoverflow.com/questions/5420317/reading-and-writing-binary-file
    binaryIn.seekg(0, ios::end);
    auto fileSize = binaryIn.tellg();
    binaryIn.seekg(sizeof(totalBits), ios::beg);
    
    // reserve space
    size_t dataBytes = static_cast<size_t>(fileSize) - sizeof(totalBits);
    byteBuffer.resize(dataBytes);
    
    // read data
    if (dataBytes > 0) 
    {
        binaryIn.read(reinterpret_cast<char*>(byteBuffer.data()), dataBytes);
        if (binaryIn.fail()) 
        {
            cout << endl;
            cout << "Error: Failed to read encoded data!" << endl;
            cout << endl;
            return 1;
        }
        binaryIn.close();
    }

    // convert to bits
    bitString.reserve(static_cast<size_t>(totalBits));
    uint64_t bitsRead = 0;
    for (size_t i = 0; i < byteBuffer.size() && bitsRead < totalBits; i++) 
    {
        unsigned char byte = byteBuffer[i];
        for (int b = 7; b >= 0 && bitsRead < totalBits; b--) 
        {
            bool bit = (byte >> b) & 1;
            bitString.push_back(bit ? '1' : '0');
            bitsRead++;
        }
    }

    return 0;
}

//
// Decodes the bits using the Huffman tree
//
int decodeBits(char* outFileName, HuffmanNode* root, const string& bitString) 
{
    // open file
    ofstream outFile(outFileName, ifstream::binary);
    if (!outFile) 
    {
        cout << endl;
        cout << "Error: Cannot open output file! " << endl;
        cout << endl;
        return 1;
    }

    // decode bit string using Huffman tree
    HuffmanNode* node = root;
    for (char bitChar : bitString) 
    {
        if (!node) break;

        if (bitChar == '0') // go left = 0
        {
            node = node->left;
        } 
        else // go right = 1
        {
            node = node->right;
        }
        // hit leaf, so output character and reset to root
        if (node && !node->left && !node->right) {
            outFile.put(node->ch);
            node = root;
        }
    }

    outFile.close();
    return 0;
}

int main(int argc, char* argv[]) {
    // 1) Read command line arguments (returns default file "decoded_output.txt")
    char* decodeTree = nullptr;
    char* encodedBin = nullptr;
    char* outputFileName = "decoded_output.txt";
    if (readArgs(argc, argv, decodeTree, encodedBin) != 0) {
        return 1;
    }
    cout << "Read arguments..." << endl;

    // 2) Read Huffman tree
    HuffmanNode* root = nullptr;
    if (readTree(decodeTree, root) != 0) {
        return 1;
    }
    cout << "Read Huffman tree..." << endl;

    // 3) Read binary file
    uint64_t totalBits;
    vector<unsigned char> byteBuffer;
    string bitString;
    if (readBinaryFile(encodedBin, totalBits, byteBuffer, bitString) != 0) {
        return 1;
    }
    cout << "Read binary file..." << endl;

    // 4) Decode bits using Huffman tree
    if (decodeBits(outputFileName, root, bitString) != 0) {
        return 1;
    }
    cout << "Decoded bits to decoded_output.txt..." << endl;

    // done
    return 0;
}
