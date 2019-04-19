
#include <cassert>
#include <sstream>
#include <cmath>
#include <chrono>
#include <utility>
#include "PillarsCracker.hpp"
#include "Parser.hpp"
#include "Utils.hpp"
#include "StructureCracker.hpp"
#include "GenerationCracker.hpp"
#include "generationByCubitect/generator.hpp"
#include "Random.hpp"

void test_data(const std::string &filename) {
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
    gen(global_data.biome, 123, global_data.option->version);
}

void test_generation() {
    const Globals global_data = parse_file("data_example.txt");
    initBiomes();
    LayerStack g = setupGenerator(global_data.option->version);
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

void genTestAgain(int64_t seed) {
    const Globals global_data = parse_file("data.txt");
    initBiomes();
    LayerStack g = setupGenerator(global_data.option->version);
    applySeed(&g, seed);
    int *map = allocCache(&g.layers[g.layerNum - 1], 1, 1);
    bool flag = true;
    int sum = 0;
    for (Biomess el:global_data.biome) {
        Pos pos;
        pos.x = el.cx;
        pos.z = el.cz;
        genArea(&g.layers[g.layerNum - 1], map, pos.x, pos.z, 1, 1);
        int biomeID = map[0];
        flag = flag && (biomeID == el.id);

        std::cout << biomeID << std::endl;
        sum++;
    }
    free(map);

}

void genDebug(int64_t partial) {
    std::vector<unsigned long long> final_seeds;
    const Globals global_data = parse_file("data.txt");
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    initBiomes();
    LayerStack g = setupGenerator(global_data.option->version);
    for (unsigned int i = 0; i < (1U << 16u); i++) {
        applySeed(&g, (int64_t) (((unsigned long long) i) << 48u | partial));
        int *map = allocCache(&g.layers[g.layerNum - 1], 1, 1);
        int sum = 0;
        for (Biomess el:global_data.biome) {
            Pos pos;
            pos.x = el.cx;
            pos.z = el.cz;
            genArea(&g.layers[g.layerNum - 1], map, pos.x, pos.z, 1, 1);
            int biomeID = map[0];
            if (biomeID == el.id) {
                sum++;
            }
        }
        free(map);
        if (sum > 3) {
            unsigned long long seedf = ((((unsigned long long) i) << 48u) | partial);
            std::cout << "Final seed found : " << (int64_t) seedf << " " << sum << std::endl;

            final_seeds.push_back(seedf);
        }
    }
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds for the seed : " << partial << std::endl;
    freeGenerator(g);


}

void structureTest() {
    const Globals global_data = parse_file("data.txt");
    int64_t seed = (-930953330566209700 & 0xFFFFFFFFFFFFD);
    int sum = 0;
    for (auto el:global_data.structures_array) {
        auto r = Random(seed + el.incompleteRand);
        long signed k = -1, m = -1;
        switch (el.typeStruct) {
            case 's': //old structures: igloo, witch hut, desert temple, jungle temple, village
                k = r.nextInt(24);
                m = r.nextInt(24);
                break;
            case 'e': //end cities
                k = (r.nextInt(9) + r.nextInt(9)) / 2;
                m = (r.nextInt(9) + r.nextInt(9)) / 2;
                break;
            case 'o': //ocean monuments
                k = (r.nextInt(27) + r.nextInt(27)) / 2;
                m = (r.nextInt(27) + r.nextInt(27)) / 2;
                break;
            case 'm': //mansions
                k = (r.nextInt(60) + r.nextInt(60)) / 2;
                m = (r.nextInt(60) + r.nextInt(60)) / 2;
                break;
            case 'r': //ruins
                k = (r.nextInt(8) + r.nextInt(8)) / 2;
                m = (r.nextInt(8) + r.nextInt(8)) / 2;
                break;
            case 'w': //shipwreck
                k = (r.nextInt(8) + r.nextInt(8)) / 2;
                m = (r.nextInt(8) + r.nextInt(8)) / 2;
                break;
            case 't': //treasures
                if (r.nextFloat() < 0.01) {
                    sum += 1;
                    std::cout << el.chunkX << el.typeStruct << std::endl;
                }
            default:
                std::cerr << "Wow that's a parser mistake pls enter in contact with your local helper: NEIL#4879" << std::endl;
                break;
        }
        if ((((el.chunkX % el.modulus) + el.modulus) % el.modulus) == k && m == (((el.chunkZ % el.modulus) + el.modulus) % el.modulus)) {
            sum += 1;
            std::cout << el.chunkX << el.typeStruct << std::endl;
        }

    }

    std::cout<<sum<<std::endl;
}

void tests() {
    return;
    std::cout << "hey" << std::endl;
    genTestAgain((int64_t) 22736);
    genDebug((int64_t) 22736);
    return;
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
