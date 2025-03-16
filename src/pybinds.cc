#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>
#include "BinaryReader.h"

namespace py = pybind11;

struct ParticleData {
    double mass;
    double p0;
    double pz;
    int32_t pdgid;
    int32_t ncoll;

    void read(std::ifstream& bfile) {
        bfile.read(reinterpret_cast<char*>(&mass), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&p0), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&pz), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&pdgid), sizeof(int32_t));
        bfile.read(reinterpret_cast<char*>(&ncoll), sizeof(int32_t));
    }
};

py::list get_particle_objects(const std::string& filename) {
    py::gil_scoped_acquire gil; // Must hold GIL for Python calls

    std::ifstream bfile(filename, std::ios::binary);
    BinaryReader<ParticleData> reader;
    reader.readHeader(bfile);
    reader.read(bfile);
    bfile.close();

    auto particle_module = py::module::import("sparkx");
    auto particle_class = particle_module.attr("Particle");

    py::list particles;

  for (const auto& pdata : reader.getAllParticles()) {
        py::object particle = particle_class();
        particle.attr("mass") = pdata.mass;
        particle.attr("E") = pdata.p0;
        particle.attr("pz") = pdata.pz;
        particle.attr("pdg") = pdata.pdgid;
        particle.attr("ncoll") = pdata.ncoll;

        particles.append(particle);
    }

    return particles;  // Now returns Python objects safely
}
// pybind11 module
PYBIND11_MODULE(binary_reader, m) {
    m.def("get_particle_objects", &get_particle_objects, 
          "Read particle data and return Python Particle objects");
}
