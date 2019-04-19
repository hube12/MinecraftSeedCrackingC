#include <chrono>
#include <fstream>
#include <csignal>

#include "Random.hpp"
#include "Utils.hpp"
#include "Parser.hpp"
#include "StructureCracker.hpp"

bool can_it_be_there(unsigned long long currentSeed, int index, std::vector<Structure> arrayStruct) {
    Structure el = arrayStruct[index];
    auto r = Random(currentSeed + el.incompleteRand);
    long signed k=-1, m=-1;
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
                if (index > 3) {
                    printf("Good seed: %llu \n", currentSeed);
                }
                if (index == (int) arrayStruct.size() - 1) {
                    return true;
                }
                return can_it_be_there(currentSeed, index + 1, arrayStruct);
            }
            return false;
        default:
            std::cerr<<"Wow that's a parser mistake pls enter in contact with your local helper: NEIL#4879"<<std::endl;
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
                           const std::vector<Structure> *arrayStruct, int processes, pid_t pidMain,std::vector<int*> pipes) {
    close(pipes[thread_id][0]); //close reading end
    std::ofstream file;
    file.open("log_process" + std::to_string(thread_id), std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for (unsigned long i = 0; i < n_iter; i++) {
            if (((i + 1) % (n_iter / 100)) == 0 && 0 != kill(pidMain, 0)) {
                file.close();
                exit(0);
            }
            unsigned long long currentSeed = time_machine(i + a[thread_id], pillar_seed);
            if (can_it_be_there(currentSeed, 0, arrayStruct[thread_id])) {
                printf("Partial seed found: %llu\n", currentSeed);
                file << currentSeed << std::endl;
            }

            if (((i + 1) % (n_iter / 100)) == 0 && thread_id==0) {
                std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
                        t2 - t1);
                std::string percentage=std::to_string(((double)i/(double)n_iter)*100.0);
                std::string current_status="We are on thread  "+ std::to_string(thread_id)+ " at: "+ percentage+"% and it took me "+std::to_string(time_span.count())+" seconds." ;
                write(pipes[thread_id][1],current_status.c_str(),current_status.size()+1);
            }
        }
        file.close();
        close(pipes[thread_id][1]); //close writing end
    } else {
        throw std::runtime_error("log file was not loaded: " + std::to_string(thread_id));
    }

}

