#pragma once

#include <vector>
#include <unistd.h>

#include "Parser.hpp"

void cuda_handler(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct);

std::vector<unsigned long long> cuda_assemble_logs();
