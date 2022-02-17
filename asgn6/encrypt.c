#include <gmp.h>
#include <stdio.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {
    //initialize file information
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pub;
    char *pub_path = "rsa.pub";

    bool verbose = false;
    char msg[1024] = "SYNOPSIS\n"
                     "   Encrypts data using RSA encryption.\n"
                     "   Encrypted data is decrypted by the decrypt program.\n\n"
                     "USAGE\n"
                     "   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n\n"
                     "OPTIONS\n"
                     "   -h              Display program help and usage.\n"
                     "   -v              Display verbose program output.\n"
                     "   -i infile       Input file of data to encrypt (default: stdin).\n"
                     "   -o outfile      Output file for encrypted data (default: stdout).\n"
                     "   -n pbfile       Public key file (default: rsa.pub).\n";

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
            //open infile
        case 'i':
            infile = fopen(optarg, "r");
            break;

            //open outfile
        case 'o':
            outfile = fopen(optarg, "w");
            break;

            //get public key file path only if optarg exists
            //do not want to overwrite default otherwise
        case 'n':
            if (optarg) {
                pub_path = optarg;
            }
            break;
        case 'v': verbose = true; break;
        case 'h':
            printf("%s", msg);
            //close files if already opened
            if (infile) {
                fclose(infile);
            }
            if (outfile) {
                fclose(outfile);
            }
            return 0;
        default:
            fprintf(stderr, "%s", msg);
            //close files if already opened
            if (infile) {
                fclose(infile);
            }
            if (outfile) {
                fclose(outfile);
            }
            return 1;
        }
    }
    //if infile is not opened properly, end program
    //and close outfile if needed
    if (infile == NULL) {
        fprintf(stderr, "Unable to open file.\n");
        if (outfile) {
            fclose(outfile);
        }
        return 1;
    }

    //if outfile is not opened properly, end program
    //and close infile if needed
    if (outfile == NULL) {
        fprintf(stderr, "Unable to open file.\n");
        if (infile) {
            fclose(infile);
        }
        return 1;
    }

    //open public key file
    pub = fopen(pub_path, "r+");
    //if it is not opened properly
    //close infile and outfile if needed
    if (pub == NULL) {
        fprintf(stderr, "Unable to open key file.\n");
        if (infile) {
            fclose(infile);
        }
        if (outfile) {
            fclose(outfile);
        }
        return 1;
    }
    char username[128];

    mpz_t n, e, s, name;
    mpz_inits(n, e, s, name, NULL);

    //get public key information from public key file
    rsa_read_pub(n, e, s, username, pub);

    //if verbose is specified, print out public key information
    //in both bits and decimal
    if (verbose == true) {
        printf("user = %s\n", username);
        gmp_printf("s (%u bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%u bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%u bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    //set the username into an mpz variable
    mpz_set_str(name, username, 62);
    //if signature is invalid, end program and close all files
    if (rsa_verify(name, s, e, n) == false) {
        fprintf(stderr, "Could not verify signature.\n");
        fclose(pub);
        fclose(infile);
        fclose(outfile);
        mpz_clears(n, e, s, name, NULL);
        return 1;
    }

    //encrypt contents of infile to outfile
    rsa_encrypt_file(infile, outfile, n, e);

    mpz_clears(n, e, s, name, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pub);
    return 0;
}
