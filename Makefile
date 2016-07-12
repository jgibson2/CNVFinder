CC=g++
CFLAGS=-Wall -Wextra -Wpedantic -lpthread --std=c++1y
SEQAN=/usr/lib/seqan/include/

CNVFinder-build: pthread_kmer_counter.h pthread_kmer_counter.cpp kmer_record.h CNVFinder-build-index.cpp dna_hasher.h dna_hasher.c sequence_file_reader.h sequence_file_reader.cpp
	$(CC) -O3 -o CNVFinder-build $(CFLAGS) -I$(SEQAN)  CNVFinder-build-index.cpp pthread_kmer_counter.cpp dna_hasher.c sequence_file_reader.cpp

test: pthread_kmer_counter.h pthread_kmer_counter.cpp kmer_record.h test.cpp dna_hasher.h dna_hasher.c sequence_file_reader.h sequence_file_reader.cpp
	$(CC) -O3 -o test.out $(CFLAGS) -I$(SEQAN)  test.cpp pthread_kmer_counter.cpp dna_hasher.c sequence_file_reader.cpp
