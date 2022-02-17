//credits to asgn6 documention for guidance
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

//initialize seed for future use of
//random generator function call
gmp_randstate_t state;
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    return;
}

//free memory from initializing
//the seed
void randstate_clear(void) {
    gmp_randclear(state);
    return;
}
