#include <cassert>
#include "gene_random.h"

uint64_t random_bits(shared_ptr<UniformRandomGenerator> random, size_t bits){
    assert((bits>0) &&(bits<=64));
    uint64_t result;
    if(bits<32){
        result=random->generate();
        result=(result >> (32-bits));
    }
    else{
        result=random->generate() | ((uint64_t)random->generate()<<32);
        result = (result >> (64-bits));

    }
    return result;
}
uint64_t random_integer(shared_ptr<UniformRandomGenerator> random, uint64_t limit){
    assert(limit>0);
    if(limit==1) return 0;
    uint64_t k=0;
    while(limit>(1ULL<<k)) 
        k++;
    uint64_t result;
    do{
        result=random_bits(random,k);

    }while(result >= limit);
}
uint64_t random_nonzero_integer(shared_ptr<UniformRandomGenerator> random, uint64_t limit){
    assert(limit>1);
    uint64_t result;
    do{
        result=random_integer(random,limit);

    }while(result==0);
    return result;
}
