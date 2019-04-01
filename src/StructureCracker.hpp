#pragma once

void structure_seed_single(unsigned long *a, unsigned long n_iter, int thread_id, unsigned int pillar_seed,
                           const std::vector<Structure> *arrayStruct, int processes);

bool
can_it_be_there(unsigned long long currentSeed, int index, std::vector<Structure> arrayStruct); //exposed only for test

void assemble_logs(int processes);