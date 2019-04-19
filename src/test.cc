#include "Random.hpp"
#include <iostream>
#include "Parser.hpp"
void structureTest() {
    const Globals global_data = parse_file("data3.txt");
    int64_t seed = (-930953330566209700 & 0xFFFFFFFFFFFFD);
    int sum = 0;
    for (auto el:global_data.structures_array) {
        auto r = Random(seed + el.incompleteRand);
        long signed k = -1, m = -1;
        switch (el.typeStruct) {
            case 's': //old structures: igloo, witch hut, desert temple, jungle temple, village
                k = r.nextInt(24);
                m = r.nextInt(24);
                break;
            case 'e': //end cities
                k = (r.nextInt(9) + r.nextInt(9)) / 2;
                m = (r.nextInt(9) + r.nextInt(9)) / 2;
                break;
            case 'o': //ocean monuments
                k = (r.nextInt(27) + r.nextInt(27)) / 2;
                m = (r.nextInt(27) + r.nextInt(27)) / 2;
                break;
            case 'm': //mansions
                k = (r.nextInt(60) + r.nextInt(60)) / 2;
                m = (r.nextInt(60) + r.nextInt(60)) / 2;
                break;
            case 'r': //ruins
                k = (r.nextInt(8) + r.nextInt(8)) / 2;
                m = (r.nextInt(8) + r.nextInt(8)) / 2;
                break;
            case 'w': //shipwreck
                k = (r.nextInt(8) + r.nextInt(8)) / 2;
                m = (r.nextInt(8) + r.nextInt(8)) / 2;
                break;
            case 't': //treasures
                if (r.nextFloat() < 0.01) {
                    sum += 1;
                    std::cout << el.chunkX <<" "<<el.chunkZ<< el.typeStruct << std::endl;
                }
            default:
                std::cerr << "Wow that's a parser mistake pls enter in contact with your local helper: NEIL#4879" << std::endl;
                break;
        }
        if ((((el.chunkX % el.modulus) + el.modulus) % el.modulus) == k && m == (((el.chunkZ % el.modulus) + el.modulus) % el.modulus)) {
            sum += 1;
            std::cout << el.chunkX << " "<<el.chunkZ<< el.typeStruct << std::endl;
        }

    }

    std::cout<<sum<<std::endl;
}
int main(){
    structureTest();
}