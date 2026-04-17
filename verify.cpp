#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

void readDataFromFile(const std::string& fileName, std::string& outputData){
    std::ifstream file(fileName);

    file.seekg(0, std::ios::end);
    outputData.resize(file.tellg());
    file.seekg(0, std::ios::beg);

    file.read(&outputData[0], outputData.size());

    file.close();
}


int main(){
    std::string fileName1 = "file.txt";
    std::string fileName2 = "decompresedFile.txt";

    int biggestFile;

    std::string data1;
    std::string data2;

    readDataFromFile(fileName1, data1);
    readDataFromFile(fileName2, data2);
    
    if (data1.size() < data2.size()){
        biggestFile = data2.size();
    }

    else{
        biggestFile = data1.size();
    }

    for(int i=0; i<biggestFile; ++i){
        if (data1[i] != data2[i]){
            std::cout << "bad, it doesn't match the other file, starting at pozition: " << i << std::endl;
            break;
        }
    }

    std::cout << "Analysis ready" << std::endl;


    return 0;
}