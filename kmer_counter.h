#include "kmer_record.h"
#include "dna_hasher.h"
#include <stdlib.h>
#include <pthread.h>
#include <cmath>
#include <stdint.h>
#include <unordered_map>

template <typename MerType> struct kmer_hash
{
	uint16_t k;
	std::unordered_map<std::string, MerType> hash;
	pthread_mutex_t hash_mutex;
	kmer_hash(uint16_t _k)
	{
		k = _k;
		hash_mutex = PTHREAD_MUTEX_INITIALIZER;
	}
};


template <typename MerType> struct kmer_sequence_data
{
	uint16_t k;
	std::string sequence;
	uint64_t start_coordinate;
	kmer_hash<MerType> *hash;
	kmer_sequence_data(std::string _sequence, uint16_t _k, kmer_hash<MerType>* _hash)
	{
		sequence = _sequence; k = _k; hash = _hash;
	}
};

void* count_genomic_kmers(void*);
void* count_read_kmers(void*);
