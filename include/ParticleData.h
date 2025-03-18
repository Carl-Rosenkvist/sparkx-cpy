#ifndef PARTICLEDATA_H
#define PARTICLEDATA_H


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
    const static Schema& get_schema(){return schema;}
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

#endif
