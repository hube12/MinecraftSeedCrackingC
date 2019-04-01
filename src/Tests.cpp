#include <iostream>
#include <assert.h>
#include <sstream>
#include <cmath>
#include <chrono>

#include "PillarsCracker.hpp"
#include "Parser.hpp"
#include "Utils.hpp"
#include "StructureCracker.hpp"
#include "GenerationCracker.hpp"
#include "generationByCubitect/generator.hpp"

void test_data(std::string filename) {
    const Globals global_data = parse_file(filename);
    assert (global_data.pillars_array != nullptr);
    for (Biomess el:global_data.biome) {
        assert(el.cz != NAN);
        assert(el.cx != NAN);
        assert(el.id != NAN);
    }
    for (Structure s:global_data.structures_array) {
        assert(s.chunkX != NAN);
        assert(s.chunkZ != NAN);
        assert(s.incompleteRand != NAN);
        assert(s.modulus != NAN);
        assert(s.typeStruct != (char) NULL);
    }
    std::cout << "Data test passed" << std::endl;
}

void test_structure() {
    const Globals global_data = parse_file("data_example.txt");
    unsigned int pillar_seed = find_pillar_seed(global_data.pillars_array);
    printf("Pillar seed was found and it is: %d \n", pillar_seed);
    unsigned long long seed = 35652699581184;
    if (can_it_be_there(seed, 0, global_data.structures_array)) {
        std::cout << "Well done we found the right seed" << std::endl;
    } else {
        std::cout << "Come on" << std::endl;
    }
}

void test_time_machine() {
    unsigned long long seed = 123;
    unsigned long long currentSeed = seed ^(unsigned long long) 0x5DEECE66D;
    currentSeed = (currentSeed * 0x5deece66d + 0xb) & (unsigned long long) 0xffffffffffff;
    currentSeed = (currentSeed * 0x5deece66d + 0xb) & (unsigned long long) 0xffffffffffff;
    auto pillar = (unsigned int) ((currentSeed & 0xFFFF0000) >> 16u);
    unsigned long long iterated =
            ((currentSeed & (unsigned long long) 0xFFFF00000000) >> 16u) | (currentSeed & (unsigned long long) 0xFFFF);
    if (time_machine(iterated, pillar) == seed) {
        std::cout << "Bravo the time machine works" << std::endl;
    } else {
        std::cout << "Come on, you are dumb" << std::endl;
    }
}

void test_pillars() {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    unsigned int seed = 1000;
    int liste[10];
    for (int i = 0; i < 10; i++) {
        liste[i] = i * 3 + 76;
    }
    shuffling(seed, liste);
    printf("%d is the seed found whereas the correct one was: %d, i guess i can say congrats!\n",
           find_pillar_seed(liste), seed);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds.";
    std::cout << std::endl;
}


void test_gen() {
    const Globals global_data = parse_file("data_example.txt");
    gen(global_data.biome, 123, global_data.option.version);
}

void test_generation() {
    const Globals global_data = parse_file("data_example.txt");
    initBiomes();
    LayerStack g = setupGenerator(global_data.option.version);
    applySeed(&g, 123);
    int *map = allocCache(&g.layers[g.layerNum - 1], 1, 1);
    bool flag = true;
    for (Biomess el:global_data.biome) {
        Pos pos;
        pos.x = el.cx;
        pos.z = el.cz;
        genArea(&g.layers[g.layerNum - 1], map, pos.x, pos.z, 1, 1);
        int biomeID = map[0];

        flag = flag && (biomeID == el.id);
    }
    if (flag) {
        std::cout << "Well done we are sure our generation works" << std::endl;
    } else {
        std::cout << "Oh no he is retarded" << std::endl;
    }
    free(map);
    freeGenerator(g);
}

void tests() {
    test_data("data_example.txt");
    printf("----------\n");
    test_time_machine();
    printf("----------\n");
    test_pillars();
    printf("----------\n");
    test_generation();
    printf("----------\n");
    test_structure();
    printf("----------\n");
    test_gen();
    printf("----------\n");
}


int main() {
    tests();
    return 0;
}
