#include "Utils.hpp"
#include <cmath>

int return_id(std::vector <pid_t> &array) {
    int retval = 0;
    for (int i = 0; i < (int) array.size(); i++) {
        retval += (array[i] > 0 ? 1 : 0) * (int) pow(2, i);
    }
    return retval;
}

unsigned long long time_machine(unsigned long seed, unsigned int pillar_seed) {
    unsigned long long currentSeed;
    currentSeed = (seed << 16u & (unsigned long long) 0xFFFF00000000) | (pillar_seed << 16u) |
                  (seed & (unsigned long long) 0xFFFF);
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & (unsigned long long) 0xffffffffffff;
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & (unsigned long long) 0xffffffffffff;
    currentSeed ^= 0x5DEECE66DU;
    return currentSeed;
}

std::vector<Structure> *
array_of_struct(int num_of_threads, const std::vector<Structure>& arrayStruct, std::vector<Structure> *a) {
    for (int i = 0; i < num_of_threads; i++) {
        for (Structure el :arrayStruct) {
            Structure s(el.chunkX, el.chunkZ, el.incompleteRand, el.modulus, el.typeStruct);
            a[i].push_back(s);
        }
    }
    return a;
}