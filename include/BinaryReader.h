#ifndef BINARYREADER_H
#define BINARYREADER_H
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
class BaseParticle {

 virtual void read(std::ifstream& bfile) = 0;

};


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
        events.push_back(Event<ParticleType>());
        int event_counter = 0;

        while (bfile.read(&blockType, sizeof(blockType))) {
            switch (blockType) {
                case 'p': {
                    ParticleBlock<ParticleType> p_block;
                    p_block.read(bfile);
                    if (checkNext(bfile)) {
                        events[event_counter].particle_blocks.push_back(p_block);
                    }
                    break;
                }

                case 'i': {
                    InteractionBlock<ParticleType> i_block;
                    i_block.read(bfile);
                    if (checkNext(bfile)) {
                        events[event_counter].interaction_blocks.push_back(i_block);
                    }
                    break;
                }

                case 'f': {
                    EndBlock e_block;
                    e_block.read(bfile);
                    events[event_counter].end_block = e_block;
                    events.push_back(Event<ParticleType>());
                    event_counter++;
                    break;
                }

                default:
                    // Handle unknown blockType or other cases if needed
                    break;
            }
        }
    }

    std::vector<Event<ParticleType>> events;
};
#endif
