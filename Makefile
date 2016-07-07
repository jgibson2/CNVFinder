CC=g++
CFLAGS=-Wall -Wextra -Wpedantic -lpthread --std=c++1y

testmake: kmer_counter.h kmer_counter.cpp kmer_record.h test.cpp dna_hasher.h dna_hasher.c
	$(CC) -o test.out $(CFLAGS) test.cpp kmer_counter.cpp dna_hasher.c
