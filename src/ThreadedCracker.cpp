#include <thread>

#include "ThreadedCracker.hpp"
#include "StructureCracker.hpp"
#include "Utils.hpp"

unsigned long long threaded_structure(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct) {
    static const int num_of_threads = 8;
    std::cout << num_of_threads << std::endl;
    std::thread threads[num_of_threads];
    unsigned long a[num_of_threads];
    unsigned long iota = (unsigned long)((1LLU<<32u)-1) / num_of_threads;
    std::vector<Structure> a_struct[num_of_threads];
    array_of_struct(num_of_threads, arrayStruct, a_struct);
    for (int i = 0; i < num_of_threads; i++) {
        a[i] = iota * i;
    }
    for (int i = 0; i < num_of_threads; ++i) {
        threads[i] = std::thread(structure_seed_single, a, iota, i, pillar_seed, a_struct);
    }
    for (auto &t:threads) {
        t.join();
    }
    for (unsigned long i = (num_of_threads - 1) * iota + iota; i < (unsigned long)((1LLU<<32u)-1); i++) {
        unsigned long long currentSeed = time_machine(i, pillar_seed);
        if (can_it_be_there(currentSeed, 0, arrayStruct)) {
            printf("Seed found: %llu\n", currentSeed);
            return currentSeed;
        }
    }
    return 0;
}



unsigned long long test_them_all(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct) {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (unsigned long i = 0; i < (unsigned long)((1LLU<<32u)-1); i++) {
        unsigned long long currentSeed = time_machine(i, pillar_seed);
        if (can_it_be_there(currentSeed, 0, arrayStruct)) {
            return currentSeed;
        }
        if ((i % (1LLU << 28u)) == 0) {
            std::cout << "We are at: " << (double) (i) / (double) ((unsigned long)((1LLU<<32u)-1)) << " " << i << " " << (unsigned long)((1LLU<<32u)-1) << std::endl;
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            std::cout << "It took me " << time_span.count() << " seconds.";
            std::cout << std::endl;
        }
    }
    return UINT_FAST64_MAX;
}

