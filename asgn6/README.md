# Assignment 6: Public Key Cryptography  
This assignment should enable the user to create public and private keys
and store them in the respective public and private files. 
After, the program should use those keys to encrypt (using public key) 
an input file, and decrypt (using private key) an output file back to
the original message.  
  
## Building  
This assignment is built with:  
$ clang -Wall -Wextra -Werror -Wpedantic $(shell pkg-config --cflags gmp) -c *.o $(shell pkg-config --libs gmp)  
  
## Running  
Keygen is ran with the command:  
$ ./keygen [-hv] [-b bits] [-i confidence] -n pbfile -d pvfile [-s seed] 
  
Encrypt is ran with the command:  
$ ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey    
   
Decrypt is ran with the command:    
$ ./decrypt [-hv] [-i infile] [-o outfile] -n privkey  
    
## Errors  
For 'make prime', I noticed that urandomb generates numbers up to, rather than at least, n bits.
Therefore, I ended up implementing for that myself, using a lot of math.
A lot of others seem to not consider that, so I mentioned it here just in case
my program somehow proves to be messed up later. For now, everything still seems to be working fine.

Also, I noticed that if the 'bits' passed into 'make prime' is one, that 'make prime' will go 
into an infinite loop because the only number ever generated would be one, which is not prime.
To take care of that, I added one to 'p' because that would be two, which is the closest prime,
and it still fits the decription of at least 'bits' bits.  
  


