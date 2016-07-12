#include <string>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cassert>
#include <iostream>
#include "dna_hasher.h"
#include "kmer_record.h"
#include "pthread_kmer_counter.h"
#include "sequence_file_reader.h"

#define NUM_THREADS 8

#define DEBUG

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		std::cerr << "Need 2 args: 1. file to read, 2. kmer length" << std::endl;
		return 1;
	}
	SequenceFileReader reader(argv[1]);	
	std::vector<kmer_hash<genomic_kmer>> hashes(NUM_THREADS, (uint8_t)atoi(argv[2]));
	kmer_hash<genomic_kmer> master_hash((uint8_t)atoi(argv[2]));
	uint64_t pos = 0;
	uint8_t k = (uint8_t)atoi(argv[2]);
	#ifdef DEBUG
	uint64_t iteration = 0;
	#endif


	while(!reader.atEnd())
	{
		std::vector<kmer_sequence_data<genomic_kmer>> data;
		for(int d = 0; d < NUM_THREADS; d++)
		{
			std::string dna = reader.getRecordBlock(10000, k);
			std::cout << dna << std::endl;
			data.push_back(kmer_sequence_data<genomic_kmer>(dna, (uint8_t)atoi(argv[2]), pos, &hashes[d]));
			pos += dna.length();
		}
		pthread_t threads[NUM_THREADS];
		for(uint16_t i = 0; i < NUM_THREADS; i++)
		{
			pthread_create(&(threads[i]), NULL, count_genomic_kmers, (void*)&data[i]);
		}
		for(uint16_t i = 0; i < NUM_THREADS; i++)
		{
		
		#ifdef DEBUG
		std::cout << "Joining thread " << i  <<std::endl;
		//std::cout << "Thread dna: " << data[i].sequence  <<std::endl;
		#endif
			pthread_join(threads[i], NULL);
		
		}
		#ifdef DEBUG
		std::cout << iteration  <<std::endl;
		iteration++;
		#endif
	}
	
	for(int i = 0; i < NUM_THREADS; i++)
	{
		for(auto kmer_record : hashes[i].hash_map)
		{
			if(master_hash.hash_map.count(kmer_record.first))
			{
				master_hash.hash_map.at(kmer_record.first).isUnique=false;
			}
			else
			{
				master_hash.hash_map.emplace(kmer_record.first, kmer_record.second);
			}
		}
	}
	#ifdef DEBUG
		for(auto kv_pair : master_hash.hash_map)
		{
			std::cout << kv_pair.first <<  " " << kv_pair.second.isUnique << " " << kv_pair.second.firstPosition << std::endl;
		}	
	#endif
	return 0;
}
