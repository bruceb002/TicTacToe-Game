#include <stdio.h>
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {
    int opt = 0;
    //default values
    uint64_t bits = 256;
    uint64_t iters = 50;
    uint64_t seed = time(NULL);
    bool verbose = false;

    //initialize file information
    FILE *pub;
    FILE *priv;
    char *pub_path = "rsa.pub";
    char *priv_path = "rsa.priv";

    char msg[1024]
        = "SYNOPSIS\n"
          "   Generates an RSA public/private key pair.\n\n"
          "USAGE\n"
          "   ./keygen [-hv] [-b bits] [-i confidence] -n pbfile -d pvfile [-s seed]\n\n"
          "OPTIONS\n"
          "   -h              Display program help and usage.\n"
          "   -v              Display verbose program output.\n"
          "   -b bits         Minimum bits needed for public key n (default: 256).\n"
          "   -i confidence   Miller-Rabin iterations for testing primes (default: 50).\n"
          "   -n pbfile       Public key file (default: rsa.pub).\n"
          "   -d pvfile       Private key file (default: rsa.priv).\n"
          "   -s seed         Random seed for testing.\n";

    //container variable, almost useless
    char *ptr;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b':
            //strtoul won't work if we do not consider that optarg can be nothing
            if (optarg) {
                bits = strtoul(optarg, &ptr, 10);
            }
            //should not be able to handle wrong datatypes
            if (errno == EINVAL) {
                return 1;
            }
            //bits less than four should not be allowed to continue
            if (bits < 4) {
                return 1;
            }
            break;
        case 'i':
            //strtoul wont work if we don't consider that optarg can be nothing
            if (optarg) {
                iters = strtoul(optarg, &ptr, 10);
            }
            //wrong datatypes not allowed
            if (errno == EINVAL) {
                return 1;
            }
            break;
        case 'n':
            //do not want to overwrite default if optarg doesn't exist
            if (optarg) {
                pub_path = optarg;
            }
            break;
        case 'd':
            //do not want to overwrite default if optarg doesn't exist
            if (optarg) {
                priv_path = optarg;
            }
            break;
        case 's':
            //can't be a wrong datatype
            //if seed is entered in as 0, the default should still be used
            if (errno != EINVAL && ((uint64_t) optarg) != 0) {
                seed = strtoul(optarg, &ptr, 10);
            }
            break;
        case 'v': verbose = true; break;
        case 'h': printf("%s", msg); return 0;
        default: fprintf(stderr, "%s", msg); return 1;
        }
    }

    //see if public key file is opened properly
    pub = fopen(pub_path, "w");
    if (pub == NULL) {
        fprintf(stderr, "Unable to properly open file.\n");
        return 1;
    }

    //see if private key file is opened properly
    priv = fopen(priv_path, "w");
    if (priv == NULL) {
        //if private key file is not proper
        //close public key file (which will definitely be opened by now)
        fclose(pub);
        fprintf(stderr, "Unable to properly open file.\n");
        return 1;
    }

    //make sure only user has access to read, write, and/or modify private file
    int priv_num = fileno(priv);
    fchmod(priv_num, 0600);

    //initializing all things gmp library
    randstate_init(seed);
    mpz_t p, q, n, e, d, s, name;
    mpz_inits(p, q, n, e, d, s, name, NULL);

    //make public key information
    rsa_make_pub(p, q, n, e, bits, iters);
    //make private key information
    rsa_make_priv(d, e, p, q);

    char *username = "";
    //get username of user
    username = getenv("USER");
    //convert username to an mpz variable
    mpz_set_str(name, username, 62);
    //create signature
    rsa_sign(s, name, d, n);

    //write proper information to respective files
    rsa_write_pub(n, e, s, username, pub);
    rsa_write_priv(n, d, priv);

    //if the user specified for verbose printing
    if (verbose == true) {
        printf("user = %s\n", username);
        gmp_printf("s (%u bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%u bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%u bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%u bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%u bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%u bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    mpz_clears(p, q, n, e, d, s, name, NULL);
    randstate_clear();
    fclose(pub);
    fclose(priv);
    return 0;
}
