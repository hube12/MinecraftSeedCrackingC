#pragma once

#include <iostream>
#include <vector>

/*
 * DATA FORMAT
 * First line will be Minecraft version number, number of processes, biome size, river size
 * Second line will be 10 numbers between 76 and 103 (inclusive) by step of 3 all separated with commas
 * Next n lines will be structures with first the unique salt, then the modulus of the structure then the type of structure
 * then the chunkX and the chunkZ all separated by commas.
 * Line n+3 will be separator
 * Next m lines will be biomes with first the biome id then coordinate x then coordinate z
 * */


enum versions {
    MC_1_7, MC_1_8, MC_1_9, MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_13_2, MC_1_14, MC_LEG
};

class Structure {
public:
    explicit Structure(long long chunkX, long long chunkZ, long long incompleteRand, int modulus, char typeStruct) {
        this->chunkX = chunkX;
        this->chunkZ = chunkZ;
        this->incompleteRand = incompleteRand;
        this->modulus = modulus;
        this->typeStruct = typeStruct;
    };
    long long chunkX;
    long long chunkZ;
    long long incompleteRand;
    int modulus;
    char typeStruct;
};

class Options {
public:
    explicit Options(versions version, bool gpu, int processes, int biome, int river) {
        this->version = version;
        this->gpu = gpu;
        this->processes = processes;
        this->biome = biome;
        this->river = river;
    }

    versions version;
    bool gpu{};
    int processes{};
    int biome{};
    int river{};
};

class Biomess {
public:
    explicit Biomess(int id, long long cx, long long cz) {
        this->id = id;
        this->cx = cx;
        this->cz = cz;
    }

    int id;
    long long cx;
    long long cz;
};

class Globals {
public:
    Options *option;
    std::vector<Biomess> biome;
    const int *pillars_array;
    std::vector<Structure> structures_array;

    explicit Globals(const int *pillars_array, std::vector<Structure> structures_array,
                     Options *option,
                     std::vector<Biomess> biomes) {
        this->pillars_array = pillars_array;
        this->structures_array = move(structures_array);
        this->biome = move(biomes);
        this->option = option;
    }
};

typedef struct Pos {
    long long x, z;
} Pos;

Globals parse_file(const std::string &filename);

versions parse_version(std::string &s);
