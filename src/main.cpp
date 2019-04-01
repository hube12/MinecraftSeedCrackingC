#include "Parser.hpp"
#include "PillarsCracker.hpp"
#include "MultiprocessedCracker.hpp"

int main(){
    std::string filename="data_example.txt";
    const Globals global_data = parse_file(filename);
    unsigned int pillar_seed = find_pillar_seed(global_data.pillars_array);
    printf("Pillar seed was found and it is: %d \n", pillar_seed);
    multiprocess_handler(pillar_seed, global_data.structures_array,global_data.option.processes);
}