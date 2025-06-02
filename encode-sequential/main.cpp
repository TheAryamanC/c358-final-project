/* main.cpp */

//
// Performs the encoding of a .txt file
//
// Aryaman C
//

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "huffman.h"

using namespace std;
using namespace std::chrono;

//
// Reads the arguments from the command line
//
int readArgs(int argc, char* argv[], char*& inputFile)
{
    if (argc != 2)
    {
        cout << endl;
        cout << "Usage: " << argv[0] << " = <input.txt>" << endl;;
        cout << endl;
        return 1;
    }

    inputFile = argv[1];
    return 0;
}

//
// Reads the input file
//
int readInputFile(const char* inputFileName, string& content)
{
    // open file
    ifstream infile(inputFileName, ifstream::binary);
    if (!infile) 
    {
        cout << endl;
        cout << "Error: Cannot open .txt file!" << endl;
        cout << endl;
        return 1;
    }

    // read entire file into string
    ostringstream buffer;
    buffer << infile.rdbuf();
    content = buffer.str();
    infile.close();
    return 0;
}

//
// Build frequency map
//
unordered_map<char, int> buildFrequencyMap(const string& content)
{
    unordered_map<char, int> freqMap;
    for (unsigned char uc : content) 
    {
        freqMap[static_cast<char>(uc)]++;
    }
    return freqMap;
}

//
// Build Huffman tree, generate codes, and write to JSON
//
int buildHuffmanTree(unordered_map<char, int>& freqMap, unordered_map<char, string>& codes, char* treeJsonName)
{
    // build tree
    HuffmanNode* root = buildHuffmanTree(freqMap);
    if (!root) 
    {
        cout << endl;
        cout << "Error: Cannot build tree!" << endl;
        cout << endl;
        return 1;
    }

    // generate bit strings for each character
    generateCodes(root, "", codes);

    // write out
    ofstream treeOut(treeJsonName, ifstream::binary);
    if (!treeOut) 
    {
        cout << endl;
        cout << "Error: Cannot write out!" << endl;
        cout << endl;
        return 1;
    }
    writeTreeJson(root, treeOut);
    treeOut.close();
    return 0;
}

//
// Write out encoded bits to binary file
// Credit to answer in https://stackoverflow.com/questions/8329767/writing-into-binary-files
//
int writeEncodedBits(string& bitString, char* encodedBinName)
{
    // open file
    ofstream binOut(encodedBinName, ifstream::binary);
    if (!binOut) 
    {
        cout << endl;
        cout << "Error: Cannot open binary file!" << endl;
        cout << endl;
        return 1;
    }

    // first, we will write a 64-bit header indicating the total number of bits
    uint64_t bits64 = static_cast<uint64_t>(bitString.size());
    binOut.write(reinterpret_cast<const char*>(&bits64), sizeof(bits64));

    // then we write everything byte-by-byte (this is how we will also decode the binary file)
    unsigned char currentByte = 0;
    int bitCount = 0;
    for (size_t i = 0; i < bitString.size(); i++) 
    {
        currentByte <<= 1;
        if (bitString[i] == '1') 
        {
            currentByte |= 1;
        }
        bitCount++;
        if (bitCount == 8) // write byte
        {
            binOut.put(static_cast<char>(currentByte));
            currentByte = 0;
            bitCount = 0;
        }
    }

    // pad leftover bits with 0s
    if (bitCount > 0) 
    {
        currentByte <<= (8 - bitCount);
        binOut.put(static_cast<char>(currentByte));
    }

    binOut.close();
    return 0;
}

int main(int argc, char* argv[]) 
{
    // 1) Read command line arguments (returns default file "encoded_output.bin" and "tree.json")
    char* inputFileName = nullptr;
    char* treeJsonName = "tree.json";
    char* encodedBinName = "encoded_output.bin";
    if (readArgs(argc, argv, inputFileName) != 0) 
    {
        return 1;
    }
    cout << "Read arguments..." << endl;

    // 2) Read input file
    auto read_start = chrono::high_resolution_clock::now();
    string content;
    if (readInputFile(inputFileName, content) != 0) 
    {
        return 1;
    }
    auto read_end = chrono::high_resolution_clock::now();
    auto diff = read_end - read_start;
    auto duration = chrono::duration_cast<chrono::milliseconds>(diff);
    cout << "Read input file in " << duration.count() << " ms..." << endl;

    // 3) Build frequency map
    auto build_start = chrono::high_resolution_clock::now();
    unordered_map<char, int> freqMap = buildFrequencyMap(content);
    auto build_end = chrono::high_resolution_clock::now();
    diff = build_end - build_start;
    duration = chrono::duration_cast<chrono::milliseconds>(diff);
    cout << "Built frequency map in " << duration.count() << " ms..." << endl;

    // 4) Build Huffman tree and get each character's corresponding bit string, and write out
    auto tree_start = chrono::high_resolution_clock::now();
    unordered_map<char, string> codes;
    if (buildHuffmanTree(freqMap, codes, treeJsonName) != 0) 
    {
        return 1;
    }
    auto tree_end = chrono::high_resolution_clock::now();
    diff = tree_end - tree_start;
    duration = chrono::duration_cast<chrono::milliseconds>(diff);
    cout << "Built Huffman Tree in " << duration.count() << " ms..." << endl;

    // 5) Encode content into bits
    auto encode_start = chrono::high_resolution_clock::now();
    string bitString;
    for (unsigned char uc : content) 
    {
        bitString += codes[static_cast<char>(uc)];
    }
    auto encode_end = chrono::high_resolution_clock::now();
    diff = encode_end - encode_start;
    duration = chrono::duration_cast<chrono::milliseconds>(diff);
    cout << "Encoded file in " << duration.count() << " ms..." << endl;

    // 6) Write out to binary file
    auto write_start = steady_clock::now();
    if (writeEncodedBits(bitString, encodedBinName) != 0) 
    {
        return 1;
    }
    auto write_end = steady_clock::now();
    diff = write_end - write_start;
    duration = chrono::duration_cast<chrono::milliseconds>(diff);
    cout << "Wrote file in " << duration.count() << " ms..." << endl;

    // 7) Calculate % compression
    size_t originalSizeBytes = std::filesystem::file_size(inputFileName);
    size_t encodedSizeBytes = std::filesystem::file_size(encodedBinName);
    double ratio = (double) encodedSizeBytes / originalSizeBytes;
    cout << "Compression %: " << ratio << endl;

    // done
    return 0;
}
