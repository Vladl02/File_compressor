#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <chrono>
#include <thread>

/*
encoding file structure:
Header(encoding table): 
1byte(bytesPerUtf8Char), Utf8Char, 1byte(codeSize), code
###\n
binaryDatacontent
the valuable bits in the last byte(1byte)
(1byte) the number of valuable bits in the end

*/

void encodingHashFromHeader(const std::string binData, std::unordered_map<std::string, std::string>& encodingHash, int& binaryPartStartIx){
    std::string utf8Char;
    std::string encoding;
    std::string binaryDataBuffer_ascii;

    uint8_t utf8Char_length;
    uint8_t encoding_length;
    //uint8_t currByte;
    bool stop = false;

    int i = 0;


    std::cout << "start" << std::endl;

    while (!stop){

        // length utf8 string
        utf8Char_length = static_cast<uint8_t>(binData[i]);

        // save utf8 string

        for (int j = 0; j<utf8Char_length; ++j){
            ++i;
            utf8Char.push_back(binData[i]);
        }
        
        ++i;

        // length utf8 string
        encoding_length = static_cast<uint8_t>(binData[i]);

        // save encoding string
        for (int j = 0; j<encoding_length; ++j){
            ++i;
            encoding.push_back(binData[i]);
        }

        encodingHash[encoding] = utf8Char;
        //std::cout << utf8Char << "has the code: " << encoding << std::endl;
        ++i;
        // verify that it's the end of the header
        if (binData[i] == '#'){
            if ( (i+3 < binData.size()) && (binData[i+1] == '#' && binData[i+2] == '#' && binData[i+3] == '\n')){
                // set the index of the start byte of the binary part of the input file
                binaryPartStartIx = i+4;
                stop = true;
            }
            else if(binData[i+1] == '#'){
            //rise erorr
                std::cout << "invalid encoding" << std::endl;
            }
        }

        utf8Char.clear();
        encoding.clear();
    }

    
}

void decodeBinaryPart(const std::string& binData ,const int startBinaryPartIx, std::unordered_map<std::string, std::string> encodingMap, const std::string outputFileName, uint8_t lastByte){
    std::ofstream outputFile(outputFileName, std::ios::binary | std::ios::out);
    uint8_t currByte;
    int bit;
    int i;
    std::string currEncodingBuff = "";
    std::string utf8Char;

    for (int i = startBinaryPartIx; i < binData.size()-2; ++i){
        currByte = binData[i];
        for (int j = 7; j >= 0; --j){
            bit = (currByte >> j) & 1;

            if (bit == 1){
                currEncodingBuff += "1";
            }
            else{
                currEncodingBuff += "0";
            }

            // verify whether the sequence is in the hashMap
            if (encodingMap.find(currEncodingBuff) != encodingMap.end()){
                utf8Char =  encodingMap[currEncodingBuff];

                outputFile.write(utf8Char.data(), utf8Char.size());
                currEncodingBuff.clear();
            }
        }
    }

    // solve the ending
    i = binData.size() - 2;
    currByte = binData[i];
    for (int j = lastByte-1; j>=0; --j){
        bit = (currByte >> j) & 1;

        if (bit == 1){
            currEncodingBuff += "1";
            }
        else{
            currEncodingBuff += "0";
        }

        // verify whether the sequence is in the hashMap
            if (encodingMap.find(currEncodingBuff) != encodingMap.end()){
                utf8Char =  encodingMap[currEncodingBuff];

                
                std::cout << utf8Char << std::endl;
                outputFile.write(utf8Char.data(), utf8Char.size());
                currEncodingBuff.clear();
            }
    }

    outputFile.close();

}


int main(){
    std::string fileDecompressName = "fileBin.bin";
    std::string outputFileName = "decompresedFile.txt";
    std::unordered_map<std::string, std::string> encodingHash;
    std::ifstream fileDecompress(fileDecompressName, std::ios::binary);
    int startBinaryPart;

    uint8_t lastByteValueableBitsCount;


    std::string data((std::istreambuf_iterator<char>(fileDecompress)), std::istreambuf_iterator<char>());

    fileDecompress.close();

    lastByteValueableBitsCount = static_cast<uint8_t>(data[data.size() - 1]);
    // read header, create hashmap, return the index of the first element from the binary file itself
    encodingHashFromHeader(data, encodingHash, startBinaryPart);

    // decoding the main binary part

    decodeBinaryPart(data, startBinaryPart, encodingHash, outputFileName, lastByteValueableBitsCount);


    return 0;
}
