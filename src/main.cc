#include "BinaryReader.h"
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cmath>

struct Field {
    enum class Type { Double, Int32 };
    std::string name;
    Type type;
    size_t offset = 0;

    Field(const std::string& n, Type t) : name(n), type(t), offset(0) {}
};

struct Schema {
    std::vector<Field> fields;
    size_t particle_size = 0;

    Schema(const std::vector<Field>& f) : fields(f) {
        compute_offsets();
    }

    Schema() = default;

    void compute_offsets() {
        particle_size = 0;
        for (auto& field : fields) {
            field.offset = particle_size;
            particle_size += (field.type == Field::Type::Double) ? sizeof(double) : sizeof(int32_t);
        }
    }
};



class ParticleData {
public:
    static Schema schema;

    static void set_schema(const Schema& s) {
        schema = s;
    }

    ParticleData() : data(schema.particle_size) {}

    void read(std::ifstream& bfile) {
        bfile.read(data.data(), schema.particle_size);
    }

    template<typename T>
    T get_field(size_t field_index) const {
        if (field_index >= schema.fields.size()) {
            throw std::out_of_range("Field index is out of bounds!");
        }

        const auto& field = schema.fields[field_index];
        size_t offset = field.offset;

        if (offset + sizeof(T) > data.size()) {
            throw std::runtime_error("Field offset is out of bounds: possible corrupt data!");
        }

        return *reinterpret_cast<const T*>(data.data() + offset);
    }

    void print() const {
        std::cout << "ParticleData {" << std::endl;
        for (size_t i = 0; i < schema.fields.size(); ++i) {
            const auto& field = schema.fields[i];

            std::cout << "  " << field.name << ": ";

            if (field.type == Field::Type::Double)
                std::cout << get_field<double>(i);
            else if (field.type == Field::Type::Int32)
                std::cout << get_field<int32_t>(i);

            std::cout << std::endl;
        }
        std::cout << "}" << std::endl;
    }

private:
    std::vector<char> data;
};
Schema ParticleData::schema{{}}; 


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
