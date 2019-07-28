#ifndef CUDA_CRACKER_RANDOM
#define CUDA_CRACKER_RANDOM
#include <stdio.h>
#include "cuda_runtime.h"

unsigned long long __device__ initSeed(unsigned long long seed)
{
    return (seed ^ (unsigned long long) 0x5deece66d) & ((1LLU << 48u) - 1);
}

long signed __device__ nextBits(unsigned long long *seed, unsigned int bits) {
	unsigned long long newSeed = 0;
    if (bits < 1) { bits = 1; }
    else if (bits > 32) { bits = 32; }
    newSeed = (*seed * 0x5deece66d + 0xb);
    newSeed &= ((1LLU << 48u) - 1);
    *seed = newSeed;
    return (long) (newSeed >> (48u - bits));
}

int __device__ nextInt(unsigned long long *seed, long unsigned bound)
{
    if (!(bound & (bound - 1))) { // bound is a power of 2
        return (long signed) ((bound * (unsigned long long) nextBits(seed, 31)) >> 31u);
    }
    else {
		long signed bits = nextBits(seed, 31);
		long signed val = bits % bound;
		while ((bits - val + (long long signed)bound - 1) < 0) { // Apparently nvcc needs the explicit casting here
			bits = nextBits(seed, 31);
			val = bits % bound;
		}
		return val;
	}
}

float __device__ nextFloat(unsigned long long *seed) {
	return nextBits(seed, 24) / (float) (1llu << 24u);
}

unsigned long long __device__ structure_seed(unsigned long seed, unsigned int pillar_seed) {
    unsigned long long currentSeed;
    currentSeed = (seed << 16u & (unsigned long long) 0xFFFF00000000) | (pillar_seed << 16u) |
                  (seed & (unsigned long long) 0xFFFF);
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & (unsigned long long) 0xffffffffffff;
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & (unsigned long long) 0xffffffffffff;
    currentSeed ^= 0x5DEECE66Du;
    return currentSeed;
}
#endif
