#pragma once

#include <vector>
#include <unistd.h>

#include "Parser.hpp"

void
multiprocess_handler(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct, int processes, pid_t pidMain,
                     std::vector<int*> pipes);

void multiprocess_structure(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct, int processes,
                            std::vector<pid_t> pids, pid_t pidMain, std::vector<int*> pipes);

std::vector<unsigned long long> assemble_logs(int processes);
