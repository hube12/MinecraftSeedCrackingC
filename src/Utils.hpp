#pragma once

#include <vector>
#include <unistd.h>
#include "Parser.hpp"

unsigned long long time_machine(unsigned long seed, unsigned int pillar_seed);

int return_id(std::vector<pid_t> &array);

std::vector<Structure> *
array_of_struct(int num_of_threads, const std::vector<Structure>& arrayStruct, std::vector<Structure> *a);