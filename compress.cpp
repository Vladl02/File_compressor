#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>

#ifdef _WIN32
#inlcude

std::string utf16ToUtf8(const std::wstring& utf16Str){
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Str(sizeNeeded, 0)
    WideCharToMultipleByte(CP_UTF8, 0, utf16Str.c_str(), -1, &utf8Str[0], sizeNeeded, nullptr, nullptr);
    return utf8Str;
}

std::string UTF8StringToWString(const std::string& utf8Str){
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    std::wstring utf16Str(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf16Str.c_str(), -1, &utf16Str[0], sizeNeeded);
    return utf16Str;
}

#endif


struct Node{
    std::string letter;
    int repetitions;
    int nodeSum;
    Node* leftChild;
    Node* rightChild;

    Node(std::string valueLetter, int valueRepetitions, int valueNodeCount): letter(valueLetter), repetitions(valueRepetitions), nodeSum(valueNodeCount), leftChild(nullptr), rightChild(nullptr){}
};

// works
std::string ReadFileContenteToUTF8String(const std::string& file_path){
    std::ifstream file(file_path, std::ios::in | std::ios::binary);

    if (!file){
        std::cout << "Can't open file!" << std::endl;
    }

    std::string data;
    file.seekg(0 , std::ios::end);
    data.resize(file.tellg());
    file.seekg(0, std::ios::beg);

    file.read(&data[0], data.size());

    file.close();

    return data;
}

// An utf8 character can have more bytes
int bytesPerCharacter(unsigned char firstByte){
    if ((firstByte & 0b10000000) == 0b00000000) return 1;
    if ((firstByte & 0b11100000) == 0b11000000) return 2;
    if ((firstByte & 0b11110000) == 0b11100000) return 3;
    if ((firstByte & 0b11111000) == 0b11110000) return 4;
    return -1;
}


void printBytesPerCharacter(const std::string & text){
    for (unsigned char c : text){
        std::cout << bytesPerCharacter(c) << ' ';
    }
}

// The hash of form: key[utf8Char]: value[number of repetitions]
std::unordered_map<std::string, int> buildHash(const std::string & fileContent){

    std::unordered_map<std::string, int> map;
    std::string utf8Char;
    int lenBuff;

    for (unsigned char c: fileContent){
        if (bytesPerCharacter(c) != -1){
            lenBuff = bytesPerCharacter(c);
            utf8Char.clear();
        }
        else {
            lenBuff -= 1;
        }

        
        utf8Char.push_back(static_cast<char>(c));

        if (lenBuff == 1){
            auto mapObj = map.find(utf8Char);
            if (mapObj != map.end()){
                map[utf8Char] += 1;
            }
            else{
                map[utf8Char] = 1;
            }
        }
    }

    return map;
}

// Create the ListNodes. Element form: <Node*, num of repetitions>
std::vector<std::pair<Node*, int>> initListNodes(const std::unordered_map<std::string, int>& map){

    std::vector<std::pair<Node*, int>> listNodes;

    for (const auto&[key, value]: map){
        Node* currNode = new Node(key, value, value);
        listNodes.push_back(std::make_pair(currNode, value));
    }

    return listNodes;
}

// Create the codification tree (using the initListNodes)
Node * createCodificationTree(const std::unordered_map<std::string, int>& map){

    std::vector<std::pair<Node*, int>> listNodes;

    listNodes = initListNodes(map);

    // works while I return to only one element that has the head node
    while (listNodes.size() > 1){

        std::sort(listNodes.begin(), listNodes.end(), [](const auto& a, const auto& b){
            return a.second > b.second;
        });

        // Ilustrates the algorithm working
        /*
        for (const auto& element: listNodes){
            std:: cout << element.second << ' ';
        }
        std::cout << std::endl;
        */

        // sum of the last two 
        int sum = listNodes[listNodes.size() - 1].second + listNodes[listNodes.size() - 2].second;
        Node* newNode = new Node("", 0, sum);
        newNode->leftChild = listNodes[listNodes.size() - 1].first;
        newNode->rightChild = listNodes[listNodes.size() - 2].first;

        //delete last two smallest elements
        listNodes.pop_back();
        listNodes.pop_back();

        //add the sum element
        listNodes.push_back(std::make_pair(newNode, sum));
    }
    
    return listNodes[0].first;
}

// Empties the memory
void deleteCodificationTree(Node* head){
    if (!head) return;

    deleteCodificationTree(head->leftChild);
    deleteCodificationTree(head->rightChild);
    
    delete head;
}

// Prints the codding table
void recursivePrintCodingTable(Node* head, std::string startingString){
    if (head && !head->leftChild && !head->rightChild){
        std::cout << head->letter << ": " << head->repetitions << ", code: " << startingString << std::endl;
        return;
    }

    recursivePrintCodingTable(head->leftChild, startingString+"0");
    recursivePrintCodingTable(head->rightChild, startingString+"1");
}

// Creates the encoding for each utf8Char
void createHashEncoding(std::unordered_map<std::string, std::string>& encodingHash, Node* head, std::string startingString){
    if (head && !head->leftChild && !head->rightChild){
        encodingHash[head->letter] = startingString;

        // the rare case when the file has only one element
        if (startingString == ""){
            encodingHash[head->letter] = "0";
        }

        return;
    }

    createHashEncoding(encodingHash, head->leftChild, startingString+"0");
    createHashEncoding(encodingHash, head->rightChild, startingString+"1");

}


void storeInBinary(const std::string & filename, std::unordered_map<std::string, std::string>& encodingHash, const std::string& utf8Content){
    
    std::ofstream binFile(filename, std::ios::binary | std::ios::out);

    int cicleController = 0;
    unsigned char buffer = 0;

    std::vector<std::pair<std::string, std::string>> orderedHash(encodingHash.begin(), encodingHash.end());

    std::sort(orderedHash.begin(), orderedHash.end(), [](const auto& a, const auto& b){
        return a.second < b.second;
    });

    // store the coding table
    for (const auto&[letter, code]: orderedHash){
        uint8_t charSize = letter.size();

        binFile.write(reinterpret_cast<const char*>(&charSize), 1);
        binFile.write(letter.data(), charSize);
        
        uint8_t codeSize = code.size();
        binFile.write(reinterpret_cast<const char*>(&codeSize), 1);

        binFile.write(code.data(), codeSize);
    }

    // delimeter between header
    std::string delimeter = "###\n";
    binFile.write(delimeter.data(), delimeter.size());


    // binary content
    std::string utf8Char;
    std::string encoding;

    unsigned char currByte;
    uint8_t byteToStore;
    uint8_t bitCounter = 0;
    bool isLastByte = false;
    int byteLen;
    int lenBuff;


    for (int i=0; i < utf8Content.size(); ++i){
        
        currByte = static_cast<unsigned char>(utf8Content[i]);

        if (i == (utf8Content.size() - 1)){
            isLastByte = true;
            
        }


        if (bytesPerCharacter(currByte) != -1){
            lenBuff = bytesPerCharacter(currByte);
            utf8Char.clear();
        }
        else {
            lenBuff -= 1;
        }

        
        utf8Char.push_back(static_cast<char>(currByte));

        if (lenBuff == 1){
            auto mapObj = encodingHash.find(utf8Char);
            if (mapObj != encodingHash.end()){
                encoding = encodingHash[utf8Char];

                

                for (char bit: encoding){


                    if (bit == '1'){
                        byteToStore <<= 1;
                        byteToStore |= 1;
                    }
                    if (bit == '0'){
                        byteToStore <<= 1;
                        byteToStore |= 0;
                    }
                    
                    bitCounter++;

                    // full byte complited, store it
                    if (bitCounter == 8){
                        bitCounter = 0;
                        binFile.write(reinterpret_cast<const char*>(&byteToStore), sizeof(byteToStore));
                        byteToStore = 0;
                    }
                }
            

            }
            else{
                break;
                // rise, wrong file format error 
            }
        if (isLastByte){
            if (bitCounter != 0){
                binFile.write(reinterpret_cast<const char*>(&byteToStore), sizeof(byteToStore));
            }
            else{
                bitCounter = 8;
            }
            binFile.write(reinterpret_cast<const char*>(&bitCounter), sizeof(bitCounter));
        }
        }
    }
    
    binFile.close();
}



int main(){
    std::string file_path = "file.txt";
    std::string output_file_name = "fileBin.bin";
    std::string utf8Content = ReadFileContenteToUTF8String(file_path);
    std::unordered_map<std::string, int> mapLetters;
    std::unordered_map<std::string, std::string> encodingHash;


    //bytesPerEachCharacter(utf8Content);
    mapLetters = buildHash(utf8Content);

    /*
    for (const auto&[key, value] : mapLetters){
        std::cout << key << ": " << value << std::endl;
    }
    */

    Node * head;

    head = createCodificationTree(mapLetters);
    createHashEncoding(encodingHash, head, "");

    storeInBinary(output_file_name, encodingHash, utf8Content);

    //recursivePrintCodingTable(head, "");
    deleteCodificationTree(head);

    return 0;
}



/*
encoding file structure:
Header(encoding table): 
1byte(bytesPerUtf8Char), Utf8Char, 1byte(codeSize), code
###\n
binaryDatacontent
the valuable bits in the last byte(1byte)
the number of valuable bits in the last byte

*/