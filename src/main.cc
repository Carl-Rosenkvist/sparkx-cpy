#include "BinaryReader.h"
#include "ParticleData.h"
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

    // ✅ Define the correct schema with field names and types
    Schema extended_schema({
        {"t", Field::Type::Double},
        {"x", Field::Type::Double},
        {"y", Field::Type::Double},
        {"z", Field::Type::Double},
        
        {"mass", Field::Type::Double},
        
        {"E", Field::Type::Double},
        {"px", Field::Type::Double},
        {"py", Field::Type::Double},
        {"pz", Field::Type::Double},

        {"pdg", Field::Type::Int32},
        {"id", Field::Type::Int32},
        {"charge", Field::Type::Int32},
        {"ncoll", Field::Type::Int32},

        {"form_time", Field::Type::Double},
        {"xsecfac", Field::Type::Double},

        {"proc_id_origin", Field::Type::Int32},
        {"proc_type_origin", Field::Type::Int32},

        {"time_last_coll", Field::Type::Double},

        {"pdg_mother1", Field::Type::Int32},
        {"pdg_mother2", Field::Type::Int32},
        {"baryon_number", Field::Type::Int32},
        {"strangeness", Field::Type::Int32}
    });

    // ✅ Compute offsets correctly before using the schema
    extended_schema.compute_offsets();
    ParticleData::set_schema(extended_schema);

    std::string file_path = argv[1];
    std::ifstream bfile(file_path, std::ios::binary);

    if (!bfile.is_open()) {
        throw std::runtime_error("File not found or could not be opened: " + file_path);
    }

    BinaryReader<ParticleData> reader;
    std::cout << "Reading header in " << file_path << "\n";
    reader.readHeader(bfile);

    reader.read(bfile);
    auto data = reader.getAllParticles(); // ✅ Corrected function name
    bfile.close();

    // ✅ Print the first particle for debugging
    if (!data.empty()) {
        std::cout << "First particle:\n";
        data[0].print();
    } else {
        std::cout << "No particle data found in file.\n";
    }

    return 0;
}
