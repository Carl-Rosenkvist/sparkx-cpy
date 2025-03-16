#include "BinaryReader.h"




bool checkNext(std::ifstream& bfile ){
  char blockType;
  bfile.read(reinterpret_cast<char*>(&blockType),sizeof(char));
  if(blockType == 'p' || blockType == 'f' || blockType == 'i'){ 
    bfile.seekg(-1, std::ios_base::cur);
    return true;
  }
  else return false;
}







