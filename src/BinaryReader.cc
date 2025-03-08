#include "BinaryReader.h"
void readHeader(std::ifstream& bfile) {
    char magic_number[5]; // 4 characters + null terminator
    uint16_t format_version;
    uint16_t format_variant;
    uint32_t len;

    // Read header fields
    bfile.read(magic_number, sizeof(char) * 4);
    magic_number[4] = '\0'; // Null-terminate the string

    bfile.read(reinterpret_cast<char*>(&format_version), sizeof(uint16_t));
    bfile.read(reinterpret_cast<char*>(&format_variant), sizeof(uint16_t));
    bfile.read(reinterpret_cast<char*>(&len), sizeof(uint32_t));

    char smash_version[len + 1]; // Allocate memory for smash_version (+1 for null terminator)
    bfile.read(smash_version, len);
    smash_version[len] = '\0'; // Null-terminate the string

    // For demonstration purposes, print the header information
    std::cout << "Magic Number: " << magic_number << std::endl;
    std::cout << "Format Version: " << format_version << std::endl;
    std::cout << "Format Variant: " << format_variant << std::endl;
    std::cout << "Smash Version: " << smash_version << std::endl;
}




bool checkNext(std::ifstream& bfile ){
  char blockType;
  bfile.read(reinterpret_cast<char*>(&blockType),sizeof(char));
  if(blockType == 'p' || blockType == 'f' || blockType == 'i'){ 
    bfile.seekg(-1, std::ios_base::cur);
    return true;
  }
  else return false;
}







