//Credits to documentation for giving super clear/explicit instructions
#include <stdio.h>
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

//produces the public key
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    uint64_t pbits = 0;
    uint64_t qbits = 0;

    //while 'n' is not the correct amount of bits
    //keep generating p, q, and n
    while (true) {
        //randomly generate pbits in proper range
        pbits = (random() % (nbits / 2)) + (nbits / 4);
        //qbits is remaining bits
        qbits = nbits - pbits;

        //make prime numbers for p and q
        make_prime(p, pbits, iters);
        make_prime(q, qbits, iters);
        mpz_mul(n, p, q);
        //if n is the right amount of bits then the loop is done
        if (mpz_sizeinbase(n, 2) == nbits) {
            break;
        }
    }
    mpz_t totient, pMinusOne, qMinusOne;
    mpz_inits(totient, pMinusOne, qMinusOne, NULL);

    //calculate the totient
    mpz_sub_ui(pMinusOne, p, 1);
    mpz_sub_ui(qMinusOne, q, 1);
    mpz_mul(totient, pMinusOne, qMinusOne);

    mpz_t randNum, gcdResult;
    mpz_inits(randNum, gcdResult, NULL);

    //while the gcd is not one,
    //keep randomly generating the public key
    while (true) {
        mpz_urandomb(randNum, state, nbits);
        gcd(gcdResult, randNum, totient);

        if (mpz_cmp_ui(gcdResult, 1) == 0) {
            mpz_set(e, randNum);
            break;
        }
    }
    mpz_clears(totient, pMinusOne, qMinusOne, NULL);
    mpz_clears(randNum, gcdResult, NULL);
    return;
}

//write public key to public file
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
    return;
}

//read public key from public file
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
    return;
}

//make private key
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    //find totient of n
    mpz_t totient, pMinusOne, qMinusOne;
    mpz_inits(totient, pMinusOne, qMinusOne, NULL);
    mpz_sub_ui(pMinusOne, p, 1);
    mpz_sub_ui(qMinusOne, q, 1);
    mpz_mul(totient, pMinusOne, qMinusOne);

    //find mod inverse of e mod inverse
    //that is the private key (d)
    mod_inverse(d, e, totient);
    mpz_clears(totient, pMinusOne, qMinusOne, NULL);
    return;
}

//write private key to private file
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
    return;
}

//read private key from private file
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);
    return;
}

//encrypt a message
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

//store encrypted contents from infile into outfile
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
    arr[0] = 0xFF;

    //equivalent to 'j' from documentation
    uint64_t bytes_processed = 1;

    mpz_t m, c;
    mpz_inits(m, c, NULL);

    //while still more contents to deal with
    while (bytes_processed > 0) {
        //read more bytes from infile into buffer
        bytes_processed = fread(arr + 1, sizeof(uint8_t), k - 1, infile);
        //mpz_import (mpz_t var, count, order, size, endian, nails, buffer)
        mpz_import(m, bytes_processed + 1, 1, sizeof(uint8_t), 1, 0, arr);

        //encrypt message
        rsa_encrypt(c, m, e, n);
        //put encrypted contents into outfile
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    mpz_clears(m, c, NULL);
    free(arr);
    return;
}

//decrypt a message
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

//decrypt contents from infile into outfile
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));

    //equivalent to variable 'j' from documention
    uint64_t bytes_processed = 0;

    mpz_t c, m;
    mpz_inits(c, m, NULL);

    //while not at the end of the file
    while (!feof(infile)) {
        //get encrypted contents from infile
        bytes_processed = gmp_fscanf(infile, "%Zx\n", c);
        //decrypt message
        rsa_decrypt(m, c, d, n);
        //put message back in buffer
        mpz_export(arr, &bytes_processed, 1, sizeof(uint8_t), 1, 0, m);
        //write decrypted content into outfile
        fwrite((arr + 1), sizeof(uint8_t), bytes_processed - 1, outfile);
    }
    mpz_clears(c, m, NULL);
    free(arr);
    return;
}

//create a signature
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

//verify the signature is valid
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    //m is only valid if it is the same as t
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
