
#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

static inline int next(int64_t *seed, const int bits)
{
    *seed = (*seed * 0x5deece66d + 0xb) & ((1LL << 48) - 1);
    return (int) (*seed >> (48 - bits));
}

static inline int nextInt(int64_t *seed, const int n)
{
    int bits, val;
    const int m = n - 1;

    if((m & n) == 0) return (int) ((n * (long)next(seed, 31)) >> 31);
    int pm=-1;
    do {
        pm++;
        bits = next(seed, 31);
        val = bits % n;
    }
    while(bits - val + m < 0);
    return pm;
}
int return_id(std::vector<pid_t> &array) {
    int retval = 0;
    for (int i = 0; i < (int)array.size(); i++) {
        retval += (array[i] > 0 ? 1 : 0) * (int) pow(2, i);
    }
    return retval;
}

int main(){
    pid_t pid1 = fork();
    pid_t pid2 = fork();
    pid_t pid3 = fork();
    std::vector<pid_t> pids;
    pids.push_back(pid1);
    pids.push_back(pid2);
    pids.push_back(pid3);
    int r=return_id(pids);
    long count=0;
    for (int64_t i=81604378624;i<(1LL<<40);i++){
        int64_t j=i+r*(1LL<<40);
        int tempo=nextInt(&j,27);
        if (tempo>1){
            std::cout<<i+r*(1LL<<40)<<" "<<count<<" "<< tempo<<std::endl;
            count++;
        }
        if (!(i%(1LL<<32))) std::cout<<i<<"loop"<<std::endl;
    }
    std::cout<<r<<" count"<<count<<std::endl;
}