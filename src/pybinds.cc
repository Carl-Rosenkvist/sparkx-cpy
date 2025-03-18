#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include "BinaryReader.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

namespace py = pybind11;


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

    py::dict to_pydict() const {
        py::dict particle_dict;
        for (size_t i = 0; i < schema.fields.size(); ++i) {
            const auto& field = schema.fields[i];
            if (field.type == Field::Type::Double)
                particle_dict[field.name.c_str()] = get_field<double>(i);
            else if (field.type == Field::Type::Int32)
                particle_dict[field.name.c_str()] = get_field<int32_t>(i);
        }
        return particle_dict;
    }

private:
    std::vector<char> data;
};
Schema ParticleData::schema{{}}; 

py::list get_particle_objects(const std::string& filename, const std::vector<Field>& fields) {
    py::gil_scoped_acquire gil; 

    Schema schema(fields);
    schema.compute_offsets();
    ParticleData::set_schema(schema);

    std::ifstream bfile(filename, std::ios::binary);

    BinaryReader<ParticleData> reader;
    reader.readHeader(bfile);
    reader.read(bfile);
    bfile.close();

    auto& particles = reader.getAllParticles();

    auto particle_module = py::module::import("sparkx");
    auto particle_class = particle_module.attr("Particle");

    py::list particle_list;

    for (const auto& pdata : particles) {
        py::object particle = particle_class();
        py::dict pdata_dict = pdata.to_pydict();

        for (auto item : pdata_dict) {
            particle.attr(item.first.cast<std::string>().c_str()) = item.second;
        }

        particle_list.append(particle);
    }

    return particle_list;
}

PYBIND11_MODULE(binary_reader, m) {
    py::enum_<Field::Type>(m, "FieldType")
        .value("Double", Field::Type::Double)
        .value("Int32", Field::Type::Int32);

    py::class_<Field>(m, "Field")
        .def(py::init<const std::string&, Field::Type>())
        .def_readwrite("name", &Field::name)
        .def_readwrite("type", &Field::type);


    m.def("get_particle_objects", &get_particle_objects,
          "Read particle data and return Python Particle objects");
}
