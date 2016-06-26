#include <kmer_record.h>
#include <dna_hasher.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector.h>

struct kmer_sequence_data
{
	std::string sequence;
	uint64_t start_coordinate;
	kmer_hash *hash;
}

template <typename MerType> struct kmer_hash
{
	uint16_t k;
	std::vector<MerType> hash;
	pthread_mutex_t hash_mutex;
}

void kmer_hash_init(kmer_hash&, uint16_t);

void* count_kmers(void*);
