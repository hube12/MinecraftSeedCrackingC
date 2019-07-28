#include "Parser.hpp"
#include "CudaRandom.cu"
#include "cuda_runtime.h"

// always positive modulo, ie range 0 to <mod-1>
#define MOD(val,mod) ((mod+(val%mod))%mod)

struct StructureInfo {
	long long chunkX;
	long long chunkZ;
	long long incompleteRand;
	int modulus;
	int nextIntBound; // 0: check float, >0: 2x randInt(bound), <0: 2x randInt(-bound)
};

void structure_vector_to_array(std::vector<Structure> arrayStruct, struct StructureInfo *array) {
	int i = 0;
	for (Structure el :arrayStruct) {
		array[i].chunkX	= el.chunkX;
		array[i].chunkZ	= el.chunkZ;
		array[i].incompleteRand = el.incompleteRand;
		array[i].modulus = el.modulus;

		switch (el.typeStruct) {
			case 's': //old structures: igloo, witch hut, desert temple, jungle temple, village
				array[i].nextIntBound=-24;
				break;
			case 'w': //shipwreck
				array[i].nextIntBound=-8;
				break;
			case 'e': //end cities
				array[i].nextIntBound=9;
				break;
			case 'o': //ocean monuments
				array[i].nextIntBound=27;
				break;
			case 'm': //mansions
				array[i].nextIntBound=60;
				break;
			case 'r': //ruins
				array[i].nextIntBound=8;
				break;
			case 't': //treasures
				array[i].nextIntBound=0; // special case since it uses float
				break;
		}
		i++;
	}
}

int __device__ validate_seed(unsigned long long seed, struct StructureInfo *structs, int num_structs, int max_fails) {
	struct StructureInfo *s;
	unsigned long long workingSeed;
	int mod, bound, k, m, matches=0, fails=0;
	for (int sid=0; sid<num_structs; sid++) {
		s = structs+sid;
		bound = s->nextIntBound;
		workingSeed = initSeed(seed + s->incompleteRand);
		if (!bound) { // treasure
			if (nextFloat(&workingSeed)<0.01) {
				matches++;
				continue;
			}
		}
		else{
			if (bound<0) { // shipwreck and old structures (igloo, witch hut, desert temple, jungle temple, village)
				k = nextInt(&workingSeed, -bound);
				m = nextInt(&workingSeed, -bound);
			}
			else { // new structures: end city, ocean monument, mansion, ruin
				k = (nextInt(&workingSeed, bound) + nextInt(&workingSeed, bound)) / 2;
				m = (nextInt(&workingSeed, bound) + nextInt(&workingSeed, bound)) / 2;
			}
			mod = s->modulus;
			if (k==MOD(s->chunkX, mod) && m==MOD(s->chunkZ, mod)) {
				matches++;
				continue;
			}
			fails++;
			if (fails>=max_fails) {
				return matches;
			}
		}
	}
	// success
	return matches;
}
