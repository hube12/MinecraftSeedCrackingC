#include <stdio.h>
#include <fstream>
#include <chrono>
#include "CudaRandom.cu"
#include "CudaStructure.cu"
#include "cuda_runtime.h"

#define MAX_CANDIDATES 16
#define NUM_BLOCKS 1024
#define BLOCK_SIZE 1024
#define SEEDS_PER_BLOCK (1LLU << 32u)/(NUM_BLOCKS*BLOCK_SIZE)

__device__ unsigned long long d_seed_candidates[MAX_CANDIDATES];
__device__ unsigned long long d_candidate_count = 0;

__device__ void add_candidate(unsigned long long seed)
{
    unsigned long long index = atomicAdd(&d_candidate_count, 1); // return old value, perfect for array index
    if (index<MAX_CANDIDATES) {
        d_seed_candidates[index] = seed;
        printf("Found candidate %llu: %llu\n", index+1, seed);
    }
}

__global__ void searchKernel(unsigned int pillar_seed, struct StructureInfo *array, int num_structs, int max_fails)
{
    unsigned long long startSeed = (blockIdx.x*blockDim.x+threadIdx.x)*SEEDS_PER_BLOCK;
    unsigned long long currentSeed;
    int matches;
    for (unsigned long long offset=0; offset<SEEDS_PER_BLOCK; offset++) {
        currentSeed = structure_seed(startSeed+offset, pillar_seed);
        matches = validate_seed(currentSeed, array, num_structs, max_fails);
        if (matches==num_structs) {
            add_candidate(currentSeed);
        }
        else if (matches>3){
            printf("Good Seed: %llu matches %d Structures\n", currentSeed, matches);
        }
    }
}


void cuda_handler(unsigned int pillar_seed, const std::vector<Structure> &arrayStruct)
{
    int max_fails = 1; // up to one structure can be wrong

    int num_structs = arrayStruct.size();
    size_t array_size_bytes = num_structs*sizeof(struct StructureInfo);
    struct StructureInfo *array = (struct StructureInfo *)malloc(array_size_bytes);
    struct StructureInfo *d_array;
    unsigned long long candidate_count;
    unsigned long long *seed_candidates;
    std::ofstream file;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // Convert to normal c structs
    structure_vector_to_array(arrayStruct, array);

    // Copy to GPU
    cudaMalloc((void **)&d_array, array_size_bytes);
    cudaMemcpy(d_array, array, array_size_bytes, cudaMemcpyHostToDevice);

    // run search
    searchKernel<<<NUM_BLOCKS,BLOCK_SIZE>>>(pillar_seed, d_array, num_structs, max_fails);
    cudaDeviceSynchronize();



    file.open("log_process0", std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        // Retrieve candidate count
        cudaMemcpyFromSymbol(&candidate_count, d_candidate_count, sizeof(unsigned long long));
        printf("Found %llu candidates\n", candidate_count);
        if (candidate_count) {
            if (candidate_count>MAX_CANDIDATES) {
                candidate_count = MAX_CANDIDATES;
                printf("GPU only stored the first %llu\n", MAX_CANDIDATES);
            }
            // Retrieve candidates
            seed_candidates = (unsigned long long *) malloc(candidate_count*sizeof(unsigned long long));
            cudaMemcpyFromSymbol(seed_candidates, d_seed_candidates, candidate_count*sizeof(unsigned long long));
            for (int i=0; i<candidate_count; i++) {
                file << seed_candidates[i] << std::endl;
            }
        }
        cudaDeviceReset();
        file.close();
    } else {
        throw std::runtime_error("log file was not loaded");
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds"<< std::endl;

    exit(0);
}

std::vector<unsigned long long> cuda_assemble_logs() {
    std::vector<unsigned long long> partials_possible_seed;
    std::remove("final_log.txt");
    std::ofstream log("final_log.txt", std::ios_base::out | std::ios::app);
    if (log.is_open()) {
        std::ifstream partial_log("log_process0", std::ios_base::in);
        if (partial_log.is_open()) {
            std::string line;
            while (std::getline(partial_log, line)) {
                log<<line<<std::endl;
                partials_possible_seed.push_back(std::stoull(line));
            }
            partial_log.close();
            std::remove("log_process0");
            bool failed = !std::ifstream("log_process0");
            if (!failed) { std::perror("Error deleting file"); std::cout<<"Error deleting file"<<std::endl;}

        } else {
            throw std::runtime_error("log file was not loaded");
        }
        log.close();
    } else {
        throw std::runtime_error("Log main file was not loaded");
    }
    std::cout<<"Log file was recompiled"<<std::endl;
    return partials_possible_seed;
}

