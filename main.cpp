#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>
#include <unistd.h>

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
        while ((bits - val + bound - 1) < 0) {
            bits = next(31);
            val = bits % bound;
        }
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

class Globals {
public:
    explicit Globals(const int *pillars_array, vector<Structure> structures_array) {
        this->pillars_array = pillars_array;
        this->structures_array = move(structures_array);
    }

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

unsigned long long time_machine(unsigned long long seed, unsigned int pillar_seed) {
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
        k = (r.nextInt(60) + r.nextInt(60));
        m = (r.nextInt(60) + r.nextInt(60));
    }
    if (el.chunkX % el.modulus == k && m == el.chunkZ % el.modulus) {
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
        if (((i) % (n_iter / 10)) == 0) {
            cout << "We are at: " << (double) (i + a[thread_id]) / (double) (maxi) << endl;
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            std::cout << "It took me " << time_span.count() << " seconds." << endl;
        }
    }
}

vector<Structure> *array_of_struct(int num_of_threads, vector<Structure> arrayStruct, vector<Structure> *a) {

    for (int i = 0; i < num_of_threads; i++) {
        for (int j = 0; j < arrayStruct.size(); j++) {
            Structure el = arrayStruct[j];
            Structure s(el.chunkX, el.chunkZ, el.incompleteRand, el.modulus, el.typeStruct);
            a[i].push_back(s);
        }
    }
    return a;
}
unsigned long long multiprocess_structure(unsigned int pillar_seed, const vector<Structure> arrayStruct){
    pid_t pid1=fork();
    pid_t pid2=fork();

    static const int num_of_process = 4;
    unsigned long a[num_of_process];
    unsigned long iota = maxi / num_of_process;
    vector<Structure> a_struct[num_of_process];
    array_of_struct(num_of_process, arrayStruct, a_struct);
    for (int i = 0; i < num_of_process; i++) {
        a[i] = iota * i;
    }
    structure_seed_single( a, iota, (pid1==0 && pid2==0)?0:((pid1==0 && pid2>0)?1:((pid1>0 && pid2==0)?2:3)), pillar_seed, a_struct);
}
unsigned long long threaded_structure(unsigned int pillar_seed, const vector<Structure> arrayStruct) {
    static const int num_of_threads = 8;
    std::thread threads[num_of_threads];
    unsigned long a[num_of_threads];
    unsigned long iota = maxi / num_of_threads;
    vector<Structure> a_struct[num_of_threads];
    array_of_struct(num_of_threads, arrayStruct, a_struct);
    for (int i = 0; i < num_of_threads; i++) {
        a[i] = iota * i;
    }
    for (int i = 0; i < num_of_threads; ++i) {
        threads[i] = std::thread(structure_seed_single, a, iota, i, pillar_seed, a_struct);
    }
    for (int i = 0; i < num_of_threads; ++i) {
        threads[i].join();
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
    long long salt;
    int modulus;
    long long incompleteRand;
    char typeStruct;
    long long chunkX, chunkZ;
    while (getline(in, line)) {
        stringstream ss(line);

        getline(ss, field, ',');
        salt = stoll(field);
        getline(ss, field, ',');
        modulus = stoi(field);
        getline(ss, field, ',');
        typeStruct = field[0];
        getline(ss, field, ',');
        chunkX = stoll(field);
        getline(ss, field, ',');
        chunkZ = stoll(field);
        if (chunkX < 0) {
            chunkX -= modulus - 1;
        }
        if (chunkZ < 0) {
            chunkZ -= modulus - 1;
        }
        incompleteRand = (chunkX / modulus * 341873128712LL + chunkZ / modulus * 132897987541LL + salt);
        data.emplace_back(Structure(chunkX, chunkZ, incompleteRand, modulus, typeStruct));
    }
    return Globals(pillar_array, data);
}

unsigned long long pieces_together() {
    const Globals global_data = parse_file();
    unsigned int pillar_seed = find_pillar_seed(global_data.pillars_array);
    printf("Pillar seed was found and it is: %d \n", pillar_seed);
    unsigned long long final_seed = multiprocess_structure(pillar_seed, global_data.structures_array);
    return final_seed;
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

int main() {
    // test_pillars();

    //printf("%llu",pieces_together());
    pieces_together();
    return 0;
}


/*DATA FORMAT
 * First line will be 10 numbers between 76 and 103 (inclusive) by step of 3 all separated with commas
 * Next n lines will be structures with first the unique salt, then the modulus of the structure then the type of structure
 * then the chunkX and the chunkZ all separated by commas.*/