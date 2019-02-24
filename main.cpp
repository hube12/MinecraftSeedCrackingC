#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <cmath>
#include <cstring>
#include <assert.h>
#include "generator.h"
#include "layers.h"
#include "finders.h"

#define maxi (unsigned long)((1LLU<<32u)-1)
using namespace std;

class Random {
private:
    unsigned long long seed;
public:
    explicit Random(unsigned long long seed) : seed((seed ^ (unsigned long long) 0x5deece66d) & ((1LLU << 48u) - 1)) {};

    void setSeed(unsigned long long seed) {
        this->seed = (seed ^ (unsigned long long) 0x5deece66d) & ((1LLU << 48u) - 1);
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

    long signed nextInt(long unsigned bound) {
        if (bound <= 0) {
            std::cerr << "Invalid bound";
        }
        if ((bound & (bound - 1)) == 0) {
            return (long signed) ((bound * (unsigned long long) next(31)) >> 31u);
        }
        long signed bits = next(31);
        long signed val = bits % bound;
        do {
            bits = next(31);
            val = bits % bound;
        }while ((bits - val + bound - 1) < 0);
        return val;

    }
};

class Structure {
public:
    explicit Structure(long long chunkX, long long chunkZ, long long incompleteRand, int modulus, char typeStruct) {
        this->chunkX = chunkX;
        this->chunkZ = chunkZ;
        this->incompleteRand = incompleteRand;
        this->modulus = modulus;
        this->typeStruct = typeStruct;
    };
    long long chunkX;
    long long chunkZ;
    long long incompleteRand;
    int modulus;
    char typeStruct;
};

class Options {
public:
    Options(string version, int processes, int biome, int river) {
        this->version = move(version);
        this->processes = processes;
        this->biome = biome;
        this->river = river;
    }
    Options(){};
    string version;
    int processes;
    int biome;
    int river;
};

class Biomes {
public:
    explicit Biomes(int id, long long cx, long long cz) {
        this->id = id;
        this->cx = cx;
        this->cz = cz;
    }

    int id;
    long long cx;
    long long cz;
};

class Globals {
public:
    explicit Globals(const int *pillars_array, vector<Structure> structures_array,
                     Options option,
                     vector<Biome> biomes) {
        this->pillars_array = pillars_array;
        this->structures_array = move(structures_array);
        this->biome = move(biomes);
        this->option = move(option);
    }

    Options option;
    vector<Biome> biome;
    const int *pillars_array;
    vector<Structure> structures_array;
};

int *shuffling(unsigned int seed, int *liste) {
    Random r = Random((unsigned long long) seed);
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
    for (unsigned int i = 0; i <= UINT16_MAX; i++) {
        if (match_pillars(i, liste)) {
            return i;
        }
    }
    //TODO throw an exception here
    return 0;
}

unsigned long long time_machine(unsigned long seed, unsigned int pillar_seed) {
    unsigned long long currentSeed;
    currentSeed = (seed << 16u & (unsigned long long) 0xFFFF00000000) | (pillar_seed << 16u) |
                  (seed & (unsigned long long) 0xFFFF);
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & (unsigned long long) 0xffffffffffff;
    currentSeed = ((currentSeed - 0xb) * 0xdfe05bcb1365) & (unsigned long long) 0xffffffffffff;
    currentSeed ^= 0x5DEECE66D;
    return currentSeed;
}

bool can_it_be_there(unsigned long long currentSeed, int index, vector<Structure> arrayStruct) {
    Structure el = arrayStruct[index];
    Random r = Random(currentSeed + el.incompleteRand);
    long signed k, m;
    if (el.typeStruct == 's') {
        k = r.nextInt(24);
        m = r.nextInt(24);
    } else if (el.typeStruct == 'e') {
        k = (r.nextInt(9) + r.nextInt(9)) / 2;
        m = (r.nextInt(9) + r.nextInt(9)) / 2;
    } else if (el.typeStruct == 'o') {
        k = (r.nextInt(27) + r.nextInt(27)) / 2;
        m = (r.nextInt(27) + r.nextInt(27)) / 2;
    } else {
        k = (r.nextInt(60) + r.nextInt(60)) / 2;
        m = (r.nextInt(60) + r.nextInt(60)) / 2;
    }
    if ((((el.chunkX % el.modulus) + el.modulus) % el.modulus) == k &&
        m == (((el.chunkZ % el.modulus) + el.modulus) % el.modulus)) {
        if (index > 3) {
            printf("Good seed: %llu \n", currentSeed);
        }
        if (index == arrayStruct.size() - 1) {
            return true;
        }
        return can_it_be_there(currentSeed, index + 1, arrayStruct);
    }
    return false;
}

unsigned long long test_them_all(unsigned int pillar_seed, const vector<Structure> &arrayStruct) {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (unsigned long i = 0; i < maxi; i++) {
        unsigned long long currentSeed = time_machine(i, pillar_seed);
        if (can_it_be_there(currentSeed, 0, arrayStruct)) {
            return currentSeed;
        }
        if ((i % (1LLU << 28u)) == 0) {
            cout << "We are at: " << (double) (i) / (double) (maxi) << " " << i << " " << maxi << endl;
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            std::cout << "It took me " << time_span.count() << " seconds.";
            std::cout << std::endl;
        }
    }
    return UINT_FAST64_MAX;
}

unsigned long long
structure_seed_single(unsigned long *a, unsigned long n_iter, int thread_id, unsigned int pillar_seed,
                      const vector<Structure> *arrayStruct) {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (unsigned long i = 0; i < n_iter; i++) {
        unsigned long long currentSeed = time_machine(i + a[thread_id], pillar_seed);
        if (can_it_be_there(currentSeed, 0, arrayStruct[thread_id])) {
            printf("Seed found: %llu\n", currentSeed);
            return currentSeed;
        }
        if ((i % (n_iter / 10)) == 0) {
            cout << "We are at: " << (double) (i + a[thread_id]) / (double) (maxi) << endl;
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            std::cout << "It took me " << time_span.count() << " seconds." << endl;
        }
    }
}


vector<Structure> *array_of_struct(int num_of_threads, vector<Structure> arrayStruct, vector<Structure> *a) {

    for (int i = 0; i < num_of_threads; i++) {
        for (Structure el :arrayStruct) {
            Structure s(el.chunkX, el.chunkZ, el.incompleteRand, el.modulus, el.typeStruct);
            a[i].push_back(s);
        }
    }
    return a;
}

int return_id(vector<pid_t> &array) {
    int retval = 0;
    for (int i = 0; i < array.size(); i++) {
        retval += (array[i] > 0 ? 1 : 0) * (int) pow(2, i);
    }
    return retval;
}

unsigned long long multiprocess_structure(unsigned int pillar_seed, const vector<Structure> &arrayStruct) {
    pid_t pid1 = fork();
    pid_t pid2 = fork();
    pid_t pid3 = fork();
    static const int num_of_process = 8;
    unsigned long a[num_of_process];
    unsigned long iota = maxi / num_of_process;
    vector<Structure> a_struct[num_of_process];
    array_of_struct(num_of_process, arrayStruct, a_struct);
    for (int i = 0; i < num_of_process; i++) {
        a[i] = iota * i;
    }
    vector<pid_t> pids;
    pids.push_back(pid1);
    pids.push_back(pid2);
    pids.push_back(pid3);
    structure_seed_single(a, iota, return_id(pids), pillar_seed, a_struct);
}

unsigned long long threaded_structure(unsigned int pillar_seed, const vector<Structure> &arrayStruct) {
    static const int num_of_threads = 8;
    cout << num_of_threads << endl;
    thread threads[num_of_threads];
    unsigned long a[num_of_threads];
    unsigned long iota = maxi / num_of_threads;
    vector<Structure> a_struct[num_of_threads];
    array_of_struct(num_of_threads, arrayStruct, a_struct);
    for (int i = 0; i < num_of_threads; i++) {
        a[i] = iota * i;
    }
    for (int i = 0; i < num_of_threads; ++i) {
        threads[i] = thread(structure_seed_single, a, iota, i, pillar_seed, a_struct);
    }
    for (auto &t:threads) {
        t.join();
    }
    for (unsigned long i = (num_of_threads - 1) * iota + iota; i < maxi; i++) {
        unsigned long long currentSeed = time_machine(i, pillar_seed);
        if (can_it_be_there(currentSeed, 0, arrayStruct)) {
            printf("Seed found: %llu\n", currentSeed);
            return currentSeed;
        }
    }
    return 0;
}


Globals parse_file() {
    string line, field;
    ifstream in("data.txt");

    //get the options
    getline(in, line);
    stringstream options(line);
    getline(options, field, ',');
    string version_number = field;
    getline(options, field, ',');
    int number_of_processes = stoi(field);
    getline(options, field, ',');
    int biome_size = stoi(field);
    getline(options, field, ',');
    int river_size = stoi(field);
    Options options_obj(version_number, number_of_processes, biome_size, river_size);

    //get the pillars array
    getline(in, line);
    stringstream pilars(line);
    int static pillar_array[10];
    int i = 0;
    while (i < 10 && getline(pilars, field, ',')) {
        pillar_array[i] = stoi(field);
        i++;
    }

    //get the structures array
    vector<Structure> data;
    string sep="--------------------------\r";
    while (getline(in, line) && sep.compare(line)) {
        stringstream structures(line);
        getline(structures, field, ',');
        long long salt = stoll(field);
        getline(structures, field, ',');
        int modulus = stoi(field);
        getline(structures, field, ',');
        char typeStruct = field[0];
        getline(structures, field, ',');
        long long chunkX = stoll(field);
        getline(structures, field, ',');
        long long chunkZ = stoll(field);
        long long incompleteRand = (((chunkX < 0) ? chunkX - (modulus - 1) : chunkX) / modulus * 341873128712LL +
                                    ((chunkZ < 0) ? chunkZ - (modulus - 1) : chunkZ) / modulus * 132897987541LL + salt);
        data.emplace_back(Structure(chunkX, chunkZ, incompleteRand, modulus, typeStruct));
    }
    //get the biomes array
    vector<Biome> biomes_obj;
    while (getline(in, line)) {
        stringstream biomes(line);
        getline(biomes, field, ',');
        int id = stoi(field);
        getline(biomes, field, ',');
        long long cx = stoll(field);
        getline(biomes, field, ',');
        long long cz = stoll(field);
        biomes_obj.emplace_back(Biomes(id, cx, cz));
    }
    return Globals(pillar_array, data, options_obj, biomes_obj);
}

unsigned long long pieces_together() {
    const Globals global_data = parse_file();
    unsigned int pillar_seed = find_pillar_seed(global_data.pillars_array);
    printf("Pillar seed was found and it is: %d \n", pillar_seed);
    unsigned long long final_seed = multiprocess_structure(pillar_seed, global_data.structures_array);
    return final_seed;
}
void test_data(){
    const Globals global_data = parse_file();
    assert (global_data.pillars_array!= nullptr);
    for (Biome el:global_data.biome){
        assert(el.cz!=NAN);
        assert(el.cx!=NAN);
        assert(el.id!=NAN);
    }
    for (Structure s:global_data.structures_array){
        assert(s.chunkX!=NAN);
        assert(s.chunkZ!=NAN);
        assert(s.incompleteRand!=NAN);
        assert(s.modulus!=NAN);
        assert(s.typeStruct!= (char)NULL);
    }
}

void test_structure() {
    const Globals global_data = parse_file();
    unsigned int pillar_seed = find_pillar_seed(global_data.pillars_array);
    printf("Pillar seed was found and it is: %d \n", pillar_seed);
    unsigned long long seed = 123;
    if (can_it_be_there(seed, 0, global_data.structures_array)) {
        cout << "Well done we found the right seed" << endl;
    } else {
        cout << "Come on" << endl;
    }
}

void test_time_machine() {
    unsigned long long seed = 123;
    unsigned long long currentSeed = seed ^(unsigned long long) 0x5DEECE66D;
    currentSeed = (currentSeed * 0x5deece66d + 0xb) & (unsigned long long) 0xffffffffffff;
    currentSeed = (currentSeed * 0x5deece66d + 0xb) & (unsigned long long) 0xffffffffffff;
    auto pillar = (unsigned int) ((currentSeed & 0xFFFF0000) >> 16u);
    unsigned long long iterated =
            ((currentSeed & (unsigned long long) 0xFFFF00000000) >> 16u) | (currentSeed & (unsigned long long) 0xFFFF);
    if (time_machine(iterated, pillar) == seed) {
        cout << "Bravo the time machine works" << endl;
    } else {
        cout << "Come on, you are dumb" << endl;
    }
}

void test_pillars() {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    unsigned int seed = 1000;
    int liste[10];
    for (int i = 0; i < 10; i++) {
        liste[i] = i * 3 + 76;
    }
    shuffling(seed, liste);
    printf("%d is the seed found whereas the correct one was: %d, i guess i can say congrats!\n",
           find_pillar_seed(liste), seed);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds.";
    std::cout << std::endl;
}

void test_generation(){
    initBiomes();
    LayerStack g = setupGenerator();
    Pos pos;
    pos.x=0;pos.z=0;
    applySeed(&g, 5);
    int biome=getBiomeAtPos(g, pos);
    printf("%d\n",biome);
    freeGenerator(g);
}
int main() {
    // test_pillars();

    //printf("%llu",pieces_together());
    //pieces_together();
    test_data();
    //test_structure();
    return 0;
}


/*DATA FORMAT
 * First line will be minecraft version number, number of processes, biome size, river size
 * Second line will be 10 numbers between 76 and 103 (inclusive) by step of 3 all separated with commas
 * Next n lines will be structures with first the unique salt, then the modulus of the structure then the type of structure
 * then the chunkX and the chunkZ all separated by commas.
 * Line n+3 will be separator
 * Next m lines will be biomes with first the biome id then coordinate x then coordinate z */