#include "Random.hpp"
#include <iostream>

int *shuffling(unsigned int seed, int *liste) {
    auto r = Random((unsigned long long) seed);
    for (unsigned int i = 10; i > 1; i--) {
        auto j = (int) r.nextInt(i);
        int temp = liste[i - 1];
        liste[i - 1] = liste[j];
        liste[j] = temp;
    }
    return liste;
}

bool match_pillars(unsigned int seed, const int *pillar_liste) {
    bool flag = true;
    int liste[10];
    for (int i = 0; i < 10; i++) {
        liste[i] = i;
    }
    shuffling(seed, liste);
    int i = 0;
    while (i < 10 && flag) {
        if ((76 + liste[i] * 3) != pillar_liste[i]) {
            flag = false;
        }
        i++;
    }
    return flag;
}

unsigned int find_pillar_seed(const int *liste) {
    for (unsigned int i = 0; i <= 3628800 ; i++) {
        if (match_pillars(i, liste)) {
            return i;
        }
    }
    std::cerr << "Out of bounds for pillar seed" << std::endl;
    throw std::range_error("Out of bounds for pillar seed\n");
}