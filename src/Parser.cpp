#include "Parser.hpp"
#include <fstream>
#include <sstream>

constexpr unsigned int str2int(const char *str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

versions parse_version(std::string &s) {
    versions v;
    switch (str2int(s.c_str())) {
        case str2int("1.7"):
            v = MC_1_7;
            break;
        case str2int("1.8"):
            v = MC_1_8;
            break;
        case str2int("1.9"):
            v = MC_1_9;
            break;
        case str2int("1.10"):
            v = MC_1_10;
            break;
        case str2int("1.11"):
            v = MC_1_11;
            break;
        case str2int("1.12"):
            v = MC_1_12;
            break;
        case str2int("1.13"):
            v = MC_1_13;
            break;
        case str2int("1.13.2"):
            v = MC_1_13_2;
            break;
        case str2int("1.14"):
            v = MC_1_14;
            break;
        default:
            v = MC_LEG;
            break;
    }
    return v;
}

Globals parse_file(std::string filename) {
    std::string line, field;

    std::ifstream datafile(filename, std::ios::in);
    if (datafile.fail() || !datafile) {
        printf("file was not loaded\n");
        throw std::runtime_error("file was not loaded");
    }
    //get the options
    std::getline(datafile, line);
    std::stringstream options(line);

    std::getline(options, field, ',');
    versions version_number = parse_version(field);
    std::getline(options, field, ',');
    int number_of_processes = stoi(field);
    if (number_of_processes < 2) {
        number_of_processes = 1;
    } else if (number_of_processes >= 8) {
        number_of_processes = 8;
    } else if (number_of_processes >= 4) {
        number_of_processes = 4;
    } else { number_of_processes = 2; }
    std::getline(options, field, ',');
    int biome_size = stoi(field);
    std::getline(options, field, ',');
    int river_size = stoi(field);
    Options options_obj(version_number, number_of_processes, biome_size, river_size);

    //get the pillars array
    std::getline(datafile, line);
    std::stringstream pilars(line);

    int static pillar_array[10];
    int i = 0;
    while (i < 10 && std::getline(pilars, field, ',')) {
        pillar_array[i] = stoi(field);
        i++;
    }

    //get the structures array
    std::vector<Structure> data;
    std::string sep = "-------------------------\r";
    while (std::getline(datafile, line) && line[0]!='-' && line[1]!='-') {
        std::stringstream structures(line);
        std::getline(structures, field, ',');
        long long salt = stoll(field);
        std::getline(structures, field, ',');
        int modulus = stoi(field);
        std::getline(structures, field, ',');
        char typeStruct = field[0];
        std::getline(structures, field, ',');
        long long chunkX = stoll(field);
        std::getline(structures, field, ',');
        long long chunkZ = stoll(field);
        switch (typeStruct) {
            case 'i': //igloo
                salt = 14357618;
                typeStruct = 's';
                break;
            case 't': //treasure
                salt = 10387320;
                break;
            case 'd': //desert temple
                salt = 14357617;
                typeStruct = 's';
                break;
            case 'e': //end city
                salt = 10387313;
                break;
            case 'j': //jungle temple
                salt = 14357619;
                typeStruct = 's';
                break;
            case 'o': //ocean monument
                salt = 10387313;
                break;
            case 'v': //village
                salt = 10387312;
                typeStruct = 's';
                break;
            case 'w': //shipwreck
                salt = 165745295;
                break;
            case 'r': //ocean ruins
                salt = 14357621;
                break;
            case 'm': //ocean ruins
                salt = 10387319;
                break;
            case 'h': //swamp hut
                salt = 14357620;
                typeStruct = 's';
                break;
            default:
                typeStruct='s';
                break;
        }

        long long incompleteRand =
                ((chunkX < 0) ? ((version_number == MC_1_13) ? (chunkX - modulus - 1) : (chunkX - (modulus - 1)))
                              : chunkX) / modulus * 341873128712LL +
                ((chunkZ < 0) ? ((version_number == MC_1_13) ? (chunkZ - modulus - 1) : (chunkZ - (modulus - 1)))
                              : chunkZ) / modulus * 132897987541LL + salt;
        data.emplace_back(Structure(chunkX, chunkZ, incompleteRand, modulus, typeStruct));

    }

    //get the biomes array
    std::vector<Biomess> biomes_obj;
    while (std::getline(datafile, line)) {
        std::stringstream bio(line);
        std::getline(bio, field, ',');
        int id = stoi(field);
        std::getline(bio, field, ',');
        long long cx = stoll(field);
        std::getline(bio, field, ',');
        long long cz = stoll(field);
        biomes_obj.emplace_back(Biomess(id, cx, cz));
    }
    return Globals(pillar_array, data, options_obj, biomes_obj);
}

