#include "BinaryReader.h"
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cmath>

class Particle : public BaseParticle{
public:
    double t,x,y,z;
    double mass;
    double p0,px,py,pz;
    int32_t pdgid,id,charge,ncoll;
    double form_time,xsecfac;
    int32_t proc_id_origin,proc_type_origin;
    double time_last_coll;
    int32_t pdg_mother1, pdg_mother2;
    int32_t baryon_number;
    int32_t strangeness;


    void read(std::ifstream& bfile) override{
        //bfile.read(reinterpret_cast<char*>(&this->t), sizeof(double));
        //bfile.read(reinterpret_cast<char*>(&this->x), sizeof(double));
        //bfile.read(reinterpret_cast<char*>(&this->y), sizeof(double));
        //bfile.read(reinterpret_cast<char*>(&this->z), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->mass), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->p0), sizeof(double));
        
        //bfile.read(reinterpret_cast<char*>(&this->px), sizeof(double));
        //bfile.read(reinterpret_cast<char*>(&this->py), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->pz), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->pdgid), sizeof(int32_t)); 
        bfile.read(reinterpret_cast<char*>(&this->ncoll), sizeof(int32_t));
        /*
        bfile.read(reinterpret_cast<char*>(&this->id), sizeof(int32_t));
        bfile.read(reinterpret_cast<char*>(&this->charge), sizeof(int32_t)); 
        bfile.read(reinterpret_cast<char*>(&this->form_time), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->xsecfac), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->proc_id_origin), sizeof(int32_t));
        bfile.read(reinterpret_cast<char*>(&this->proc_type_origin), sizeof(int32_t));
        bfile.read(reinterpret_cast<char*>(&this->time_last_coll), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&this->pdg_mother1), sizeof(int32_t));
        bfile.read(reinterpret_cast<char*>(&this->pdg_mother2), sizeof(int32_t));
        bfile.read(reinterpret_cast<char*>(&this->baryon_number), sizeof(int32_t)); 
        bfile.read(reinterpret_cast<char*>(&this->strangeness), sizeof(int32_t));
   
        */
    }



};


double rapidity(const Particle& p) {
    return 0.5 * std::log((p.pz + p.p0) / (p.p0 - p.pz));
}

double transverse_mass(const Particle& p) {
    return std::sqrt(p.p0 * p.p0 - p.pz * p.pz);
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>\n";
        return 1;
    }

    

    std::string dirPath = argv[1];
    std::ifstream bfile(dirPath + "/particles_binary.bin", std::ios::binary);

    if (!bfile.is_open()) {
        throw std::runtime_error("File not found or could not be opened");
    }

    std::cout << "Reading header in "+dirPath + "\n";
    readHeader(bfile);
    BinaryReader<Particle> reader;
    
   
    std::cout << "Reading file in "+dirPath + "\n";
    reader.read(bfile);
    std::cout << "File read complete!\n";
    std::map<int, int> unwounded_per_event;
    std::set<int> pdgs {321, 3122, 3312, 3334, 3212, -321, -3212, -3122, -3312, -3334};

    // Open a single output file
    std::ofstream csvFile(dirPath + "/particles_data.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Unable to open file for writing\n";
        return 1;
    }

    // Write header
    csvFile << "event_id,pdg_id,rapidity,transverse_mass,mass\n";

    // Process particles
    for (size_t i = 0; i < reader.events.size(); ++i) {
        for (const auto& block : reader.events[i].particle_blocks) {
            for (const auto& p : block.particles) {
                if ((p.pdgid == 2212 || p.pdgid == 2112) && p.ncoll == 0) { 
                    ++unwounded_per_event[i];
                }

                if (pdgs.find(p.pdgid) != pdgs.end()) {
                    csvFile << i << "," << p.pdgid << ","
                            << rapidity(p) << "," << transverse_mass(p) << "," << p.mass << "\n";
                }
            }
        }
    }

    csvFile.close();

    // Write unwounded nucleons separately
    std::ofstream unwoundedFile(dirPath + "/unwounded.csv");
    if (unwoundedFile.is_open()) {
        unwoundedFile << "event_id,unwounded_count\n";
        for (const auto& [event, unwounded] : unwounded_per_event) {
            unwoundedFile << event << "," << unwounded << "\n";
        }
        unwoundedFile.close();
    } else {
        std::cerr << "Unable to open unwounded file for writing\n";
    }

    return 0;
}
