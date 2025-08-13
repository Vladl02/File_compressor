#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>


int main(){
    std::string fileName = "testBin1.bin";
    std::ifstream fileBinary(fileName, std::ios::in | std::ios::binary);
    std::ofstream outputFile("clasicBinary1.txt");

    std::string content;

    fileBinary.seekg(0, std::ios::end);
    content.resize(fileBinary.tellg());
    fileBinary.seekg(0, std::ios::beg);

    fileBinary.read(&content[0], content.size());



    std::string pureBinaryEquivalent = "";
    int counter = 0;
    int bit;


    for (unsigned char byte: content){
        if (counter == 10){
            counter = 0;
            outputFile << std::endl;
        }

        for(int i = 7; i>=0; --i){
            bit = (byte >> i) & 1;
            if (bit == 1){
                pureBinaryEquivalent += "1";
            }
            else{
                pureBinaryEquivalent += "0";
            }
        }

        outputFile << pureBinaryEquivalent;
        outputFile << " ";
        counter += 1;

        pureBinaryEquivalent = "";
    }

    fileBinary.close();
    outputFile.close();


    return 0;
}
