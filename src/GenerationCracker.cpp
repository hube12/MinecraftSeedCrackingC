#include <chrono>
#include <vector>

#include "Parser.hpp"
#include "GenerationCracker.hpp"
#include "generationByCubitect/generator.hpp"

std::vector<unsigned long long> gen(std::vector<Biomess> bio, unsigned long long partial, versions version) {
    std::vector<unsigned long long> final_seeds;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    initBiomes();
    LayerStack g = setupGenerator(version);
    for (unsigned int i = 0; i < (1 << 16) - 1; i++) {
        bool flag = true;
        for (Biomess el:bio) {
            Pos pos;
            pos.x = el.cx;
            pos.z = el.cz;
            applySeed(&g, (int64_t) ((unsigned long long) i << 48 | partial));
            int *map = allocCache(&g.layers[g.layerNum - 1], 1, 1);
            genArea(&g.layers[g.layerNum - 1], map, pos.x, pos.z, 1, 1);
            int biomeID = map[0];
            free(map);
            flag = flag && (biomeID == el.id);
            if (!flag) {
                break;
            }
        }
        if (flag) {
            std::cout << "Final seed found : " << (((unsigned long long) i << 48) | partial) << std::endl;
            final_seeds.push_back((((unsigned long long) i << 48) | partial));
        }
    }
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;

    freeGenerator(g);
    return final_seeds;
}

std::vector<unsigned long long> gen_handler(std::vector<Biomess> bio, std::vector<unsigned long long> partials, versions version){
    std::vector<unsigned long long> finals_seeds;
    for (auto el:partials){
        std::vector<unsigned long long> tempo=gen(bio,el,version);
        finals_seeds.reserve(finals_seeds.size() + distance(tempo.begin(),tempo.end()));
    }
    return finals_seeds;
}