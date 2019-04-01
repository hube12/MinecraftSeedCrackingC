#include <chrono>
#include <vector>

#include "Parser.hpp"
#include "GenerationCracker.hpp"
#include "generationByCubitect/generator.hpp"

unsigned long long gen(std::vector<Biomess> bio, unsigned long long partial, versions version) {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
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
        }
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;

    freeGenerator(g);
    return 0;
}

