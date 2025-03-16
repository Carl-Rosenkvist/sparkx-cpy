#ifndef BINARYREADER_H
#define BINARYREADER_H

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>



template <typename ParticleType>
class ParticleBlock {
public:
    void read(std::ifstream& bfile) {
        int32_t event_number; //New field
        int32_t ensamble_number; //New field
        uint32_t npart;
        bfile.read(reinterpret_cast<char*>(&event_number), sizeof(uint32_t));
        bfile.read(reinterpret_cast<char*>(&ensamble_number), sizeof(uint32_t)); 
        bfile.read(reinterpret_cast<char*>(&npart), sizeof(uint32_t));
        particles.resize(npart);
        for (auto& part : particles) {
            part.read(bfile);
        }
    }


    std::vector<ParticleType> particles;
};

template <typename ParticleType>
class InteractionBlock {
public:
    void read(std::ifstream& bfile) {
        bfile.read(reinterpret_cast<char*>(&n_in), sizeof(uint32_t));
        bfile.read(reinterpret_cast<char*>(&n_out), sizeof(uint32_t));
        bfile.read(reinterpret_cast<char*>(&rho), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&sigma), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&sigma_p), sizeof(double));
        bfile.read(reinterpret_cast<char*>(&process), sizeof(uint32_t));
        inc_particles.resize(n_in);
        for (auto& part : inc_particles) {
            part.read(bfile);
        }

        out_particles.resize(n_out);
        for (auto& part : out_particles) {
            part.read(bfile);
        }
    }

private:
    std::vector<ParticleType> inc_particles;
    std::vector<ParticleType> out_particles;
    uint32_t n_in;
    uint32_t n_out;
    double rho;
    double sigma;
    double sigma_p;
    uint32_t process;
};

struct EndBlock {
    void read(std::ifstream& bfile) {
        bfile.read(reinterpret_cast<char*>(&event_number), sizeof(event_number));
        bfile.read(reinterpret_cast<char*>(&ensamble_number), sizeof(ensamble_number));
        bfile.read(reinterpret_cast<char*>(&impact_parameter), sizeof(impact_parameter));
        bfile.read(&empty, sizeof(empty));
    }

    uint32_t event_number;
    int32_t ensamble_number;
    double impact_parameter;
    char empty;
};

template <typename ParticleType>
struct Event {
    std::vector<ParticleBlock<ParticleType>> particle_blocks;
    std::vector<InteractionBlock<ParticleType>> interaction_blocks;
    EndBlock end_block;
};

void readBinary(std::ifstream& bfile);
void readHeader(std::ifstream& bfile);
bool checkNext(std::ifstream& bfile);

template <typename ParticleType>
class BinaryReader {
public:
    void read(std::ifstream& bfile) {
        char blockType;

        while (bfile.read(&blockType, sizeof(blockType))) {
            switch (blockType) {
                case 'p': {
                    ParticleBlock<ParticleType> p_block;
                    p_block.read(bfile);

                   all_particles.insert(all_particles.end(), 
                                        p_block.particles.begin(), 
                                        p_block.particles.end());
                    
                    break;
                }

                case 'i': {
                    InteractionBlock<ParticleType> i_block;
                    i_block.read(bfile);
                    break;
                }

                case 'f': {
                    EndBlock e_block;
                    e_block.read(bfile);
                    break;
                }

                default:
                    // Handle unknown blockType or other cases if needed
                    break;
            }
        }
    }

    std::vector<ParticleType>& getAllParticles() {
        return all_particles;
    }
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
   // std::cout << "Magic Number: " << magic_number << std::endl;
    //std::cout << "Format Version: " << format_version << std::endl;
    //std::cout << "Format Variant: " << format_variant << std::endl;
    //std::cout << "Smash Version: " << smash_version << std::endl;
}

private:
    std::vector<ParticleType> all_particles;
};



#endif
