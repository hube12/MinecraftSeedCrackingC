#include "MultiprocessedCracker.hpp"
#include "Utils.hpp"
#include "StructureCracker.hpp"

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
    assemble_logs(processes);

}

void multiprocess_structure(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct, int processes,
                            std::vector<pid_t> pids) {

    static const int num_of_process = processes;
    unsigned long a[num_of_process];
    unsigned long iota = (unsigned long)((1LLU<<32u)-1) / num_of_process;
    std::vector<Structure> a_struct[num_of_process];
    array_of_struct(num_of_process, arrayStruct, a_struct);
    for (int i = 0; i < num_of_process; i++) {
        a[i] = iota * i;
    }
    structure_seed_single(a, iota, return_id(pids), pillar_seed, a_struct,processes);
}

