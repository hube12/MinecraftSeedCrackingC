#include <chrono>
#include <fstream>

#include "Random.hpp"
#include "Utils.hpp"
#include "Parser.hpp"
#include "StructureCracker.hpp"

bool can_it_be_there(unsigned long long currentSeed, int index, std::vector<Structure> arrayStruct) {
    Structure el = arrayStruct[index];
    auto r = Random(currentSeed + el.incompleteRand);
    long signed k, m;
    if (el.typeStruct == 's') {
        k = r.nextInt(24);
        m = r.nextInt(24);
    } else if (el.typeStruct == 'e') {
        k = (r.nextInt(9) + r.nextInt(9)) / 2;
        m = (r.nextInt(9) + r.nextInt(9)) / 2;
    } else if (el.typeStruct == 'o') {
        k = (r.nextInt(27) + r.nextInt(27)) / 2;
        m = (r.nextInt(27) + r.nextInt(27)) / 2;
    } else {
        k = (r.nextInt(60) + r.nextInt(60)) / 2;
        m = (r.nextInt(60) + r.nextInt(60)) / 2;
    }
    if ((((el.chunkX % el.modulus) + el.modulus) % el.modulus) == k &&
        m == (((el.chunkZ % el.modulus) + el.modulus) % el.modulus)) {
        if (index > 3) {
            printf("Good seed: %llu \n", currentSeed);
        }
        if (index == (int) arrayStruct.size() - 1) {
            return true;
        }
        return can_it_be_there(currentSeed, index + 1, arrayStruct);
    }
    return false;
}

void structure_seed_single(unsigned long *a, unsigned long n_iter, int thread_id, unsigned int pillar_seed,
                           const std::vector<Structure> *arrayStruct, int processes, pid_t pidMain) {
    std::ofstream file;
    file.open("log_process" + std::to_string(thread_id), std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for (unsigned long i = 0; i < n_iter; i++) {
            if (0!=kill(pidMain,0)){
                file.close();
                exit(0);
            }
            unsigned long long currentSeed = time_machine(i + a[thread_id], pillar_seed);
            if (can_it_be_there(currentSeed, 0, arrayStruct[thread_id])) {
                printf("Partial seed found: %llu\n", currentSeed);
                file << currentSeed << std::endl;
            }

            if (((i + 1) % (n_iter / 10)) == 0 && thread_id == 0) {
                std::cout << "We are at: "
                          << ((double) (i + a[thread_id]) / (double) (unsigned long) ((1LLU << 32u) - 1)) *
                             (double) processes * 100.0;
                std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
                        t2 - t1);
                std::cout << "% and it took me " << time_span.count() << " seconds." << std::endl;
            }
        }
        file.close();
    } else {
        throw std::runtime_error("log file was not loaded: " + std::to_string(thread_id));
    }

}

