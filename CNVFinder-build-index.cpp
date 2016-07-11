#include "pthread_kmer_counter.h"
#include "kmer_record.h"
#include "sequence_file_reader.h"
#include "dna_hasher.h"
#include <stdint.h>
#include <fstream>

#define PTHREAD_STACK_SIZE 300000000


#define DEBUG

void writeIndexToFile(char* filename, kmer_hash<genomic_kmer> genomic_kmer_hash)
{
	/*
	 * Function: write unique genomic kmers to index file
	 * Args: filename: file name to write file to; genomic_kmer_hash: kmer_hash containing genetic data
	 * Notes:
	 * 	Index file format is as follows: with k being the length of the kmer, k is encoded by the first byte. Afterwards a sequence is encoded by 8 bytes and the position is encoded by 8 bytes.
	 */
	try
	{
		std::ofstream index_file(filename, std::ios::out|std::ios::binary);
		index_file.write(reinterpret_cast<const char *>(&genomic_kmer_hash.k), sizeof(genomic_kmer_hash.k));
		for(auto kmer_record : genomic_kmer_hash.hash_map)
		{
			if(kmer_record.second.isUnique)
			{
				uint64_t kmer =  hashDNAstringtonum(kmer_record.first);
				index_file.write(reinterpret_cast<const char*>(&kmer), sizeof(kmer));
				index_file.write(reinterpret_cast<const char*>(&kmer_record.second.firstPosition), sizeof(kmer_record.second.firstPosition));
			}
		}

	}
	catch(std::ios_base::failure& e)
	{
		std::cerr << "Could not write to file " << filename << ". " << std::endl;
	}
}

kmer_hash<genomic_kmer> buildUniqueGenomicKmerHash(char* filename, uint8_t k, uint16_t num_threads, uint64_t first_coordinate, uint32_t record_block_size)
{
	/*
	 * Function: build hash of unique genomic k-mers
	 * Args: filename: file name to read FASTA genome file from; k: length of k-mer; num_threads: number of threads to run counter with
	 * Notes:
	 * 	Genomic file should contain either contigs or chromosomes; reader reads in by record. Maybe change this to split large records into fixed blocks? K-mer lengths of up to 32 are supported; after this they are frankly not useful, as mostly everything is unique.
	 */
	SequenceFileReader reader(filename);
	std::vector<kmer_hash<genomic_kmer>> thread_hashes(num_threads, k);
	kmer_hash<genomic_kmer> master_hash(k);
	uint64_t coordinate = first_coordinate;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_SIZE);
	std::vector<kmer_sequence_data<genomic_kmer>> data;
	for(uint16_t i = 0; i < num_threads; i++)
	{
		data.push_back(kmer_sequence_data<genomic_kmer>(std::string(), k, coordinate, &(thread_hashes[i])));
	}
	//TODO: Make this section ...better...
	//	NOTE: Implement checking of number of active threads and 1-at-a-time incremental reading of standard-sized record blocks.
	while(!reader.atEnd())
	{	
		std::vector<pthread_t> threads(num_threads);
		for(uint16_t i = 0; i < num_threads; i++)
		{
			std::string dna = reader.getRecordBlock(record_block_size);
			pthread_mutex_lock(&(data[i].hash->hash_mutex));
			data[i].sequence = dna;
			data[i].start_coordinate = coordinate;
			pthread_create(&(threads[i]), &attr, count_genomic_kmers, (void*)&data[i]);
			pthread_mutex_unlock(&(data[i].hash->hash_mutex));
			coordinate += dna.length();
		}

		for(uint16_t i = 0; i < num_threads; i++)
		{
			#ifdef DEBUG
			std::cerr << "Joining thread " << i << "..." << std::endl;
			#endif
			pthread_join(threads[i], NULL);
		}

	}
	pthread_attr_destroy(&attr);
	//combine hashes
	#ifdef DEBUG
	std::cerr << "Finished finding unique kmers..." << std::endl;
	#endif
	for(auto t_hash: thread_hashes)
	{
		for(auto kmer_record : t_hash.hash_map)
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
	std::cerr << "Finished combining hashes..." << std::endl;
	#endif

	#ifdef DEBUG
		for(auto kv_pair : master_hash.hash_map)
		{
			//std::cout << kv_pair.first <<  " " << kv_pair.second.isUnique << " " << kv_pair.second.firstPosition << std::endl;
		}	
	#endif
	
	return master_hash;
}

void printHelp()
{
	std::string help = "\
	 * Function: main function to run unique genomic kmer index builder\n\
	 * Args:\n\
	 * 	Switches:\n\
	 * 		-p <uint16> : number of threads\n\
	 * 		-k <uint8> : k-mer length\n\
	 * 		-g <path/to/file> : genome FASTA file\n\
	 * 		-o <path/to/file> : output file\n\
	 * 		-h : help\n\
	 * 		-r <uint32> : number of FASTA records to get at once (default 1)\n\
	 * 		-c <uint64> : starting coordinate (default 0)\n\
	 * Notes:\n\
	 * 	This program must be run to build the index before the anaylsis can be performed\n\
	 ";
	std::cerr << help << std::endl;
}

int main(int argc, char **argv)
{
	int err = 0;
	uint8_t k = 0;
	uint16_t num_threads = 1;
	char* genome_filename;
	char* output_filename;
	uint32_t record_block_size = 1;
	uint64_t start_coord = 0;
	/*
	 * Function: main function to run unique genomic kmer index builder
	 * Args:
	 * 	Switches:
	 * 		-p <uint16> : number of threads
	 * 		-k <uint8> : k-mer length
	 * 		-g <path/to/file> : genome FASTA file
	 * 		-o <path/to/file> : output file
	 * 		-h : help
	 * 		-r <uint32> : number of FASTA records to get at once (default 1)
	 * 		-c <uint64> : starting coordinate (default 0)
	 * Notes:
	 * 	This program must be run to build the index before the anaylsis can be performed
	 */
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-p")==0)
		{
			num_threads = (uint16_t)atoi(argv[i + 1]);
		}	
		if(strcmp(argv[i], "-g")==0)
		{
			genome_filename = argv[i+1];
		}
		if(strcmp(argv[i], "-o")==0)
		{
			output_filename = argv[i+1];
		}
		if(strcmp(argv[i], "-h")==0)
		{
			printHelp();
			return 0;
		}
		if(strcmp(argv[i], "-r")==0)
		{
			record_block_size = (uint32_t)atoi(argv[i + 1]);
		}
		if(strcmp(argv[i], "-c")==0)
		{
			start_coord = (uint64_t)atoi(argv[i + 1]);
		}
		if(strcmp(argv[i], "-k")==0)
		{
			k = (uint8_t)atoi(argv[i + 1]);
		}
	}
	
	if(!record_block_size){err &= 1;}
	if(strcmp(genome_filename,"")==0){err &= 2;}
       	if(strcmp(output_filename,"") ==0){err &= 4;}
	if(!k){err &= 8;}
	
	if(err)
	{
		if(err | 1)
			std::cerr << "Invalid record block size " << record_block_size << std::endl;
		if(err | 2)
			std::cerr << "Invalid genome filename " << genome_filename << std::endl;
		if(err | 4)
			std::cerr << "Invalid output filename " << output_filename << std::endl;
		if(err | 8)
			std::cerr << "Invalid k " << k << std::endl;
		return err;
	}

	writeIndexToFile(output_filename, buildUniqueGenomicKmerHash(genome_filename, k, num_threads, start_coord, record_block_size));
	return 0;
}
