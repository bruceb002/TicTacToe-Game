//credits to the asgn6 documentation for pseudocode/guidance
//Credits to Eugene Chou for planning out how to go about 'is_prime' and 'make_prime'
#include <stdio.h>
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

//finds the power mod of three numbers
//aka, mods a base raised to an exponent, by a modulus
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    //initializing my own variables
    //v: temporary 'out' (outcome) variable
    //tmp_expon: temporary 'exponent' variable
    //p_base: temporary 'base' variable
    //mult_res: stands for "multiplication result"
    mpz_t v, tmp_expon, p_base, mult_res;
    mpz_inits(v, tmp_expon, p_base, mult_res, NULL);

    //make sure to use these tmp variables
    //so as to not overwrite passed in parameters
    mpz_set(tmp_expon, exponent);
    mpz_set_ui(v, 1);
    mpz_set(p_base, base);

    //while the exponent is more than zero
    while (mpz_cmp_ui(tmp_expon, 0) > 0) {
        //if exponent turns out is odd
        if (mpz_odd_p(tmp_expon) != 0) {
            //multiply the base with the current outcome
            mpz_mul(mult_res, v, p_base);
            //mod that result with 'modulus'
            //and set as the current outcome value
            mpz_mod(v, mult_res, modulus);
        }

        //square the base, then mod that value
        //and set it to the current base
        mpz_mul(mult_res, p_base, p_base);
        mpz_mod(p_base, mult_res, modulus);

        //floor divide the exponent by two
        mpz_fdiv_q_ui(tmp_expon, tmp_expon, 2);
    }

    //set final tmp outcome variable to 'out'
    mpz_set(out, v);
    mpz_clears(v, tmp_expon, p_base, mult_res, NULL);
    return;
}

//checks to see if 'n' is prime
//using 'iters' amounts of rounds to check
bool is_prime(mpz_t n, uint64_t iters) {
    //if the number is not two and it is even, it is not a prime
    if ((mpz_cmp_ui(n, 2) < 0) || (mpz_cmp_ui(n, 2) != 0 && mpz_even_p(n) != 0)) {
        return false;
    }

    //if the number is two or three, it is a prime
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        return true;
    }

    mpz_t s, r, pwr, ans, n_tmp1;
    mpz_inits(s, r, pwr, ans, n_tmp1, NULL);
    mpz_set_ui(s, 0);
    mpz_sub_ui(n_tmp1, n, 1);
    mpz_set(r, n_tmp1);

    //while r is not odd
    while (true) {
        //program for n - 1 = 2^s * r
        //until r is odd
        if (mpz_cmp_ui(s, 0) == 0) {
            mpz_set_ui(pwr, 1);
            mpz_mul(ans, pwr, r);
            mpz_add_ui(ans, ans, 1);
        } else {
            mpz_mul_ui(pwr, pwr, 2);
            mpz_mul(ans, pwr, r);
            mpz_add_ui(ans, ans, 1);
        }
        if (mpz_cmp(ans, n) == 0 && mpz_odd_p(r) != 0) {
            break;
        }
        mpz_add_ui(s, s, 1);
        mpz_fdiv_q_ui(r, r, 2);
    }

    mpz_t s_tmp, n_tmp3, y, j, a, two;
    mpz_inits(s_tmp, n_tmp3, y, j, a, two, NULL);
    mpz_sub_ui(n_tmp3, n, 3);
    mpz_sub_ui(s_tmp, s, 1);
    mpz_set_ui(two, 2);

    for (uint64_t i = 1; i <= iters; i++) {

        //urandomm already minuses one for us
        //(n-1)-3 = n-4
        //range: [0, n-4]
        mpz_urandomm(a, state, n_tmp3);

        //[0+2, n-4 + 2] = [2, n-2], the range we want
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);

        //if y is not 1 nor n-1
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_tmp1) != 0) {

            mpz_set_ui(j, 1);
            //while j is not s-1 and y is not n-1
            while (mpz_cmp(j, s_tmp) <= 0 && mpz_cmp(y, n_tmp1) != 0) {

                pow_mod(y, y, two, n);
                //if y is one, then n is not a prime number
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(s, r, pwr, ans, n_tmp1, NULL);
                    mpz_clears(s_tmp, n_tmp3, y, j, a, two, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            //if y is n-1, n is not a prime number
            if (mpz_cmp(y, n_tmp1) != 0) {
                mpz_clears(s, r, pwr, ans, n_tmp1, NULL);
                mpz_clears(s_tmp, n_tmp3, y, j, a, two, NULL);
                return false;
            }
        }
    }
    mpz_clears(s, r, pwr, ans, n_tmp1, NULL);
    mpz_clears(s_tmp, n_tmp3, y, j, a, two, NULL);
    return true;
}

//produces prime number <at least> n bits long
//math in order to have prime number be at least 'n' bits long:
//offset: 2^(n-1)
//original range using mpz_urandomb: [0, (2^n)-1]
//modified range: [0, (2^(n-1)) - 1]
//offset added: [0 + offset, (2^(n-1))-1 + offset]
//resulting range: [2^(n-1), (2^n) - 1]
//reasoning: 2^(n-1) + 2^(n-1) = 2^n, and add the '- 1' in there: (2^n) - 1
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    //offset: offsets generated number so it's at least n bits
    //pTmp: makes sure value of 'p' is not overwritten (otherwise Seg Faults)
    mpz_t offset, pTmp;
    mpz_inits(offset, pTmp, NULL);

    uint64_t bitsMinusOne = bits - 1;
    mpz_ui_pow_ui(offset, 2, bitsMinusOne);

    //while prime number not found
    while (true) {
        //if bitsMinusOne is zero, I predict the outcome could be unpredictable.
        //I am not taking a chance on that.
        if (bitsMinusOne != 0) {
            mpz_urandomb(pTmp, state, bitsMinusOne);
        }
        //add the offset
        mpz_add(pTmp, pTmp, offset);

        //if 'bits' is one, aka the only number being generated ever is one
        //set it to two, which is the closest prime
        if (mpz_cmp_ui(pTmp, 1) == 0) {
            mpz_add_ui(pTmp, pTmp, 1);
        }
        if (is_prime(pTmp, iters)) {
            break;
        }
    }
    mpz_set(p, pTmp);
    mpz_clears(offset, pTmp, NULL);
    return;
}

//finds the mod inverse of 'a' mod 'n'
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r, not_r, t, not_t, tmp, q;
    mpz_inits(r, not_r, t, not_t, tmp, q, NULL);

    //set r to n
    mpz_set(r, n);
    //set inverse of r to a
    mpz_set(not_r, a);

    //set t to zero
    mpz_set_ui(t, 0);
    //set inverse of t to 1
    mpz_set_ui(not_t, 1);

    //while inverse of r is not zero
    while (mpz_cmp_ui(not_r, 0) != 0) {
        //divide r with its inverse
        mpz_fdiv_q(q, r, not_r);

        //save value of r in 'tmp' so not to lose/overwrite it prematurely
        mpz_set(tmp, r);
        mpz_set(r, not_r);
        mpz_mul(not_r, q, not_r);

        //old value of r used here as 'tmp'
        mpz_sub(not_r, tmp, not_r);

        //save value of y in 'tmp' so not to lose/overwrite it prematurely
        mpz_set(tmp, t);
        mpz_set(t, not_t);
        mpz_mul(not_t, q, not_t);
        mpz_sub(not_t, tmp, not_t);
    }
    //if r is more than one there is no inverse
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(i, 0);
        mpz_clears(r, not_r, t, not_t, tmp, q, NULL);
        return;
    }
    //if t is less than zero, there is an inverse
    //add t and n to t
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    //the final value for t is the mod inverse
    mpz_set(i, t);
    mpz_clears(r, not_r, t, not_t, tmp, q, NULL);
    return;
}

//finds the gcd between 'a' and 'b'
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t t, aTmp, bTmp;
    mpz_inits(t, aTmp, bTmp, NULL);

    mpz_set(aTmp, a);
    mpz_set(bTmp, b);

    //while b does not equal zero
    while (mpz_cmp_ui(bTmp, 0) != 0) {
        //set b to t
        mpz_set(t, bTmp);
        //the new b value is a mod b
        mpz_mod(bTmp, aTmp, bTmp);
        //set a as t
        mpz_set(aTmp, t);
    }
    //the final a value is the gcd
    mpz_set(d, aTmp);
    mpz_clears(t, aTmp, bTmp, NULL);
    return;
}
