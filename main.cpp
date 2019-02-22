#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include <vector>
#include <sstream>

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

class Globals{
public:
    explicit Globals( const int *pillars_array,vector<Structure> structures_array){
        this->pillars_array=pillars_array;
        this->structures_array=move(structures_array);
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

bool match_pillars(unsigned int seed,const int *pillar_liste) {
    bool flag = true;
    int liste[10];
    for (int i = 0; i < 10; i++) {
        liste[i] = i;
    }
    int *shuffled_liste = shuffling(seed, liste);
    int i = 0;
    while (i < 10 && flag) {
        if ((76 + shuffled_liste[i] * 3) != pillar_liste[i]) {
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
    if (el.chunkX % el.modulus == k && m == el.chunkZ % el.modulus){
        if (index>3){
            printf("Good seed: %llu \n",currentSeed);
        }
        if (index==arrayStruct.size()-1){
            return true;
        }
        return can_it_be_there(currentSeed,index+1,arrayStruct);
    }
    return false;
}

unsigned long long test_them_all(unsigned int pillar_seed, const vector<Structure>& arrayStruct){
    //TODO threading here
    for (unsigned long i=0;i<UINT_FAST32_MAX;i++){
        unsigned long long currentSeed = time_machine(i, pillar_seed);
        if (can_it_be_there(currentSeed,0,arrayStruct)){
            return currentSeed;
        }
    }
    return UINT_FAST64_MAX;
}

Globals parse_file(){

    string line,field;
    ifstream in("data.txt");

    //get the pillars array
    getline(in,line);
    stringstream pilars(line);
    int static pillar_array[10];
    int i=0;
    while (i<10 && getline(pilars,field,',')){
        pillar_array[i]=stoi(field);
        i++;
    }

    //get the structures array
    vector <Structure> data;
    while (getline(in,line)){
        stringstream ss(line);
        getline(ss,field,',');
        long long chunkX=stoll(field);
        getline(ss,field,',');
        long long chunkZ=stoll(field);
        getline(ss,field,',');
        long long incompleteRand=stoll(field);
        getline(ss,field,',');
        int modulus=stoi(field);
        getline(ss,field,',');
        char typeStruct=field[0];
        data.emplace_back(Structure(chunkX,chunkZ,incompleteRand,modulus,typeStruct));
    }
    return Globals(pillar_array,data);
}

unsigned long long pieces_together(){
    const Globals global_data=parse_file();
    unsigned int pillar_seed=find_pillar_seed(global_data.pillars_array);
    return test_them_all(pillar_seed,global_data.structures_array);
}
void test_pillars(){
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    int liste[10] = {79, 88, 97, 85, 103, 94, 76, 91, 82, 100};
    printf("%d\n", find_pillar_seed(liste));
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "It took me " << time_span.count() << " seconds.";
    std::cout << std::endl;
}
int main() {

    return 0;
}
