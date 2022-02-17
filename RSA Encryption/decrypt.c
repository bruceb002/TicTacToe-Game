#include <stdio.h>
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {
    //initialize file information
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *priv;
    char *priv_path = "rsa.priv";

    bool verbose = false;
    int opt = 0;

    char msg[1024] = "SYNOPSIS\n"
                     "   Decrypts data using RSA decryption.\n"
                     "   Encrypted data is encrypted by the encrypt program.\n\n"
                     "USAGE\n"
                     "   ./decrypt [-hv] [-i infile] [-o outfile] -n privkey\n\n"
                     "OPTIONS\n"
                     "   -h              Display program help and usage.\n"
                     "   -v              Display verbose program output.\n"
                     "   -i infile       Input file of data to decrypt (default: stdin).\n"
                     "   -o outfile      Output file for decrypted data (default: stdout).\n"
                     "   -n pvfile       Private key file (default: rsa.priv).\n";

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
            //open both infile and outfile
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o':
            outfile = fopen(optarg, "w");
            break;

            //set the path name for the private key file
        case 'n': priv_path = optarg; break;
        case 'v': verbose = true; break;
        case 'h':
            fprintf(stdout, "%s", msg);
            //close proper files if needed
            if (infile) {
                fclose(infile);
            }
            if (outfile) {
                fclose(outfile);
            }
            return 0;
        default:
            fprintf(stderr, "%s", msg);
            //close proper files if needed
            if (infile) {
                fclose(infile);
            }
            if (outfile) {
                fclose(outfile);
            }
            return 1;
        }
    }

    //if either file is not opened properly, close the other file
    if (infile == NULL) {
        if (outfile) {
            fclose(outfile);
        }
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }
    if (outfile == NULL) {
        if (infile) {
            fclose(infile);
        }
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }

    //open the private key file
    priv = fopen(priv_path, "r");
    //if the private key file was not opened properly
    //close both infile and outfile
    if (priv == NULL) {
        fclose(infile);
        fclose(outfile);
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }

    mpz_t n, d;
    mpz_inits(n, d, NULL);
    //get the private key information from the private key file
    rsa_read_priv(n, d, priv);
    //if verbose is printing, print out the private key information
    if (verbose == true) {
        gmp_printf("n (%u bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%u bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    //decrypt the contents from infile to outfile
    rsa_decrypt_file(infile, outfile, n, d);

    mpz_clears(n, d, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(priv);
    return 0;
}
