#include "kmer_record.h"
#include "dna_hasher.h"
#include <stdlib.h>
#include <pthread.h>
#include <cmath>
#include <stdint.h>
#include <unordered_map>

template <typename MerType> struct kmer_hash
{
	uint8_t k;
	std::unordered_map<std::string, MerType> hash_map;
	pthread_mutexattr_t mut_attr;
	pthread_mutex_t hash_mutex;
	kmer_hash(uint8_t _k)
	{
		k = _k;
		pthread_mutexattr_init(&mut_attr);
		pthread_mutexattr_settype(&mut_attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&hash_mutex, &mut_attr);
	}

	kmer_hash(kmer_hash<MerType>& _kmer_hash): k(_kmer_hash.k), hash_map(_kmer_hash.hash_map) 
	{
		pthread_mutexattr_init(&mut_attr);
		pthread_mutexattr_settype(&mut_attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&hash_mutex, &mut_attr);
	}
	kmer_hash(const kmer_hash<MerType>& _kmer_hash): k(_kmer_hash.k), hash_map(_kmer_hash.hash_map) 
	{
		pthread_mutexattr_init(&mut_attr);
		pthread_mutexattr_settype(&mut_attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&hash_mutex, &mut_attr);
	}
};		


template <typename MerType> struct kmer_sequence_data
{
	uint8_t k;
	std::string sequence;
	uint64_t start_coordinate;
	kmer_hash<MerType> *hash;
	kmer_sequence_data(std::string _sequence, uint8_t _k, kmer_hash<MerType>* _hash)
	{
		sequence = _sequence; k = _k; hash = _hash;
	}
	kmer_sequence_data(std::string _sequence, uint8_t _k, uint64_t _start_coordinate, kmer_hash<MerType>* _hash)
	{
		sequence = _sequence; k = _k; start_coordinate = _start_coordinate; hash = _hash;
	}
	//do NOT free hash -- it will be used later and deallocated automagically
};

void* count_genomic_kmers(void*);
void* count_read_kmers(void*);
