#pragma once

#include <iostream>

class Random {
private:
    unsigned long long seed;
public:
    explicit Random(unsigned long long seed) : seed((seed ^ (unsigned long long) 0x5deece66d) & ((1LLU << 48u) - 1)) {};

    void setSeed(unsigned long long _seed) {
        this->seed = (_seed ^ 0x5deece66dLLU) & ((1LLU << 48u) - 1u);
    }

    unsigned long long getSeed() {
        return this->seed;
    }

    long signed next(unsigned int bits) {

        if (bits < 1) { bits = 1; }
        else if (bits > 32) { bits = 32; }
        seed = (seed * 0x5deece66d + 0xb);
        seed &= ((1LLU << 48u) - 1);
        return (long) (seed >> (48u - bits));
    }

    long signed nextInt(long  bound) {
        if (bound <= 0) {
            std::cerr << "Invalid bound";
        }
        if (((unsigned)bound & ((unsigned)bound - 1u)) == 0) {
            return (long signed) ((bound * (unsigned long long) next(31)) >> 31u);
        }
        long signed bits = next(31);
        long signed val = bits % bound;
        while ((bits - val + bound - 1) < 0) {
            bits = next(31);
            val = bits % bound;
        }
        return val;
    }

    float nextFloat() {
        return (float)next(24) / (1LLU << 24u);
    }
};
