#include <string>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <algorithm>
#include "dna_hasher.h"
#include "kmer_record.h"
#include "kmer_counter.h"

int main(int argc, char **argv)
{
	std::string dna(argv[1]);
	std::string dna2(argv[2]);
	kmer_hash<genomic_kmer> hash((uint16_t)atoi(argv[3]));
	kmer_sequence_data<genomic_kmer> data(dna, (uint16_t)atoi(argv[3]), &hash);
	pthread_t thread;
	pthread_t thread2;
	kmer_sequence_data<genomic_kmer> data2(dna2, (uint16_t)atoi(argv[3]), &hash);
	pthread_create(&thread, NULL, count_genomic_kmers, (void*)&data);
	pthread_create(&thread2, NULL, count_genomic_kmers, (void*)&data2);
	pthread_join(thread, NULL);
	pthread_join(thread2, NULL);
	for (auto kv_pair : hash.hash)
	{
		std::cout << kv_pair.first << " " << kv_pair.second.isUnique << std::endl;
	}
	return 0;
}
