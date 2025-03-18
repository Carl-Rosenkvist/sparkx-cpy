#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include "BinaryReader.h"
#include "ParticleData.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

namespace py = pybind11;

class Particle : public ParticleData {
public:
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
};

class ParticleStorer : public ParticleTypeStorer<Particle> {
public:
    ParticleStorer() {
        auto particle_module = py::module::import("sparkx");
        particle_class = particle_module.attr("Particle");
    }

    void store(const std::vector<Particle>& particles) override {
        
    
      for (const auto& particle : particles) {
            auto py_particle = particle_class();

            const auto& fields = ParticleData::get_schema().fields;
            for (size_t i = 0; i <fields.size(); i++){
              py_particle.attr(fields[i].name.c_str()) = 
         (fields[i].type == Field::Type::Double) ? particle.get_field<double>(i)
                                    : particle.get_field<int32_t>(i);

            }
            particle_list.append(py_particle);
        }
    }

    py::list get_particles() { return particle_list; }

private:
    py::object particle_class;
    py::list particle_list;
};

py::list get_particle_objects(const std::string& filename, const std::vector<Field>& fields) {
    py::gil_scoped_acquire gil;

    Schema schema(fields);
    schema.compute_offsets();
    ParticleData::set_schema(schema);

    std::ifstream bfile(filename, std::ios::binary);
    if (!bfile) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    ParticleStorer storer;
    BinaryReader<Particle> reader{storer};

    reader.read(bfile);  // Read data from file

    return storer.get_particles();  // Return Python list
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
