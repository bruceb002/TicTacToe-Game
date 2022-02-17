CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic $(shell pkg-config --cflags gmp)
LDFLAGS = $(shell pkg-config --libs gmp)
OBJK = keygen.o numtheory.o randstate.o rsa.o
OBJE = encrypt.o numtheory.o randstate.o rsa.o
OBJD = decrypt.o numtheory.o randstate.o rsa.o
all: keygen encrypt decrypt

keygen: $(OBJK)
	$(CC) $^ -o $@ $(LDFLAGS)

encrypt: $(OBJE)
	$(CC) $^ -o $@ $(LDFLAGS)

decrypt: $(OBJD)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f keygen
	rm -f encrypt
	rm -f decrypt
format:
	clang-format -i -style=file *.[ch]

