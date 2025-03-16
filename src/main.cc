#include "BinaryReader.h"
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cmath>

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>\n";
        return 1;
    }

    

    std::string file_path = argv[1];
    std::ifstream bfile(file_path, std::ios::binary);

    if (!bfile.is_open()) {
        throw std::runtime_error("File not found or could not be opened");
    }

    BinaryReader<Particle> reader;
    std::cout << "Reading header in "+file_path + "\n";
    reader.readHeader(bfile);

   
    reader.read(bfile);
    auto data = reader.getAllParticleData();
    bfile.close();
    

    return 0;
}
