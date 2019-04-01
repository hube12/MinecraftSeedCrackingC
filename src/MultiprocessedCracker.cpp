#include "MultiprocessedCracker.hpp"
#include "Utils.hpp"
#include "StructureCracker.hpp"

#include <fstream>

void multiprocess_handler(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct, int processes) {
    std::vector<pid_t> pids;
    switch (processes) {
        case 2: {
            pid_t pid1 = fork();
            pids.push_back(pid1);
            break;
        }
        case 4: {
            pid_t pid1 = fork();
            pid_t pid2 = fork();
            pids.push_back(pid1);
            pids.push_back(pid2);
            break;
        }
        case 8: {
            pid_t pid1 = fork();
            pid_t pid2 = fork();
            pid_t pid3 = fork();
            pids.push_back(pid1);
            pids.push_back(pid2);
            pids.push_back(pid3);
            break;
        }
        default: {
            processes = 1;
        }
    }
    multiprocess_structure(pillar_seed, arrayStruct, processes, pids);
}

void multiprocess_structure(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct, int processes,
                            std::vector<pid_t> pids) {

    static const int num_of_process = processes;
    unsigned long a[num_of_process];
    unsigned long iota = (unsigned long) ((1LLU << 32u) - 1) / num_of_process/2;
    std::vector<Structure> a_struct[num_of_process];
    array_of_struct(num_of_process, arrayStruct, a_struct);
    for (int i = 0; i < num_of_process; i++) {
        a[i] = iota * i;
    }
    structure_seed_single(a, iota, return_id(pids), pillar_seed, a_struct, processes);
}

std::vector<unsigned long long> assemble_logs(int processes) {
    std::vector<unsigned long long> partials_possible_seed;
    std::ofstream log("final_log.txt", std::ios_base::out | std::ios::trunc);
    if (log.is_open()) {
        for (auto thread_id = 0; thread_id < processes; thread_id++) {
            std::ifstream partial_log("log_process" + std::to_string(thread_id), std::ios_base::in);
            if (partial_log.is_open()) {
                std::string line;
                while (std::getline(partial_log, line)) {
                    log<<line<<std::endl;
                    partials_possible_seed.push_back(std::stoull(line));
                }
                partial_log.close();
                std::remove(("log_process" + std::to_string(thread_id)).c_str());
                bool failed = !std::ifstream("log_process" + std::to_string(thread_id));
                if (!failed) { std::perror("Error deleting file"); std::cout<<"Error deleting file"<<std::endl;}
            } else {
                throw std::runtime_error("log file was not loaded: " + std::to_string(thread_id));
            }
        }
        log.close();
    } else {
        throw std::runtime_error("log main file was not loaded:");
    }
    std::cout<<"log file was recompiled"<<std::endl;
    return partials_possible_seed;
}
