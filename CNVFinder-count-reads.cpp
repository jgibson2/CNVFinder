#include "pthread_kmer_counter.h"
#include "kmer_record.h"
#include "sequence_file_reader.h"
#include "dna_hasher.h"
#include <stdint.h>
#include <fstream>

#define PTHREAD_STACK_SIZE 300000000
#define MAX_PATH_LENGTH 300

#define DEBUG

struct IndexFileData
{
	uint8_t k;
	kmer_hash<read_kmer> uniques_hash;
	IndexFileData(uint8_t _k, kmer_hash<read_kmer> _uniques_hash) : k(_k), uniques_hash(_uniques_hash)
	{
	}
};

IndexFileData readIndexFile(char* filename)
{
	/*
	 * Function: read data from generated index file
	 * Args: filename : filename of index
	 * Notes:
	 * 	Index file generated using CNVFinder-build
	 */
	uint8_t k;
	try
	{
		std::ifstream index_file(filename, std::ios::in|std::ios::binary);
		index_file.read(reinterpret_cast<char *>(&k), 1);
		kmer_hash<read_kmer> uniques_hash(k);
		while(!index_file.eof())
		{
			uint64_t kmer, location;
			index_file.read(reinterpret_cast<char*>(&kmer), 8);
			index_file.read(reinterpret_cast<char*>(&location), 8);
			uniques_hash.hash_map.emplace(hashDNAnumtostring(kmer,k), read_kmer(location, 0));
		}
		index_file.close();
		IndexFileData filedata(k,uniques_hash);
		return filedata;

	}
	catch(std::ios_base::failure& e)
	{
		std::cerr << "Could not read from file " << filename << ". " << std::endl;
	}

	IndexFileData filedata(0, kmer_hash<read_kmer>(0));
	return filedata;
}

kmer_hash<read_kmer> countUniqueKmersInReads(char* filename, uint8_t k, uint16_t num_threads, kmer_hash<read_kmer> unique_kmer_hash, uint32_t records)
{
	/*
	 * Function: get counts of unique kmers from built index in short reads
	 * Args: filename: file name to read FASTA/Q short read file from; k: length of k-mer; num_threads: number of threads to run counter with, unique_kmer_hash: hash of unique kmers from index file with counts initialized to zero, records: number of records to read in at once
	 * Notes:
	 * 	Genomic index file is built using CNVFinder-build
	 */
	SequenceFileReader reader(filename);
	std::vector<kmer_hash<read_kmer>> thread_hashes(num_threads, unique_kmer_hash);
	kmer_hash<read_kmer> master_hash(unique_kmer_hash);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_SIZE);
	std::vector<kmer_sequence_data<read_kmer>> data;
	for(uint16_t i = 0; i < num_threads; i++)
	{
		data.push_back(kmer_sequence_data<read_kmer>(std::string(), k, 0, &(thread_hashes[i])));
	}
	//TODO: Make this section ...better...
	//	NOTE: Implement checking of number of active threads and 1-at-a-time incremental reading of standard-sized record blocks.
	while(!reader.atEnd())
	{	
		std::vector<pthread_t> threads(num_threads);
		for(uint16_t i = 0; i < num_threads; i++)
		{
			std::string dna = reader.getRecords(records);
			pthread_mutex_lock(&(data[i].hash->hash_mutex));
			data[i].sequence = dna;
			pthread_create(&(threads[i]), &attr, count_read_kmers, static_cast<void*>(&data[i]));
			pthread_mutex_unlock(&(data[i].hash->hash_mutex));
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
	std::cerr << "Finished counting unique kmers..." << std::endl;
	#endif

	for(auto t_hash: thread_hashes)
	{
		for(auto kmer_record : t_hash.hash_map)
		{
			master_hash.hash_map.at(kmer_record.first).count += kmer_record.second.count;
		}
	}
	#ifdef DEBUG
	std::cerr << "Finished combining hashes..." << std::endl;
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
	 * 		-f <path/to/file> : short reads file (FASTA/FASTQ)\n\
	 * 		-i <path/to/file> : index file\n\
	 * 		-h : help\n\
	 * 		-o <path/to/file> : output file\n\
	 * 		-r <uint32> : number of FASTA/Q records to get at once (default 1)\n\
	 * Notes:\n\
	 * 	This program must be run to build the index before the anaylsis can be performed\n\
	 ";
	std::cerr << help << std::endl;
}

void writeCountsFile(char* filename, kmer_hash<read_kmer> uniques_hash_counted)
{
	try
	{
		std::ofstream outfile(filename, std::ios::out);
		outfile << "Kmer\tPos\tCount\n";
		for(auto kmer_rec : uniques_hash_counted.hash_map)
		{
			outfile << kmer_rec.first << "\t" << kmer_rec.second.firstPosition << "\t" << kmer_rec.second.count << "\n";
		}
	}
	catch(std::ios_base::failure& e)
	{
		std::cerr << "Could not write to file " << filename << ". " << std::endl;
	}
}

int main(int argc, char **argv)
{
	int err = 0;
	uint16_t num_threads = 1;
	char index_filename[MAX_PATH_LENGTH] = "";
	char reads_filename[MAX_PATH_LENGTH] = "";
	char output_filename[MAX_PATH_LENGTH] = "";
	uint32_t records = 100;
	/*
	 * Function: main function to run unique genomic kmer index builder
	 * Args:
	 * 	Switches:
	 * 		-p <uint16> : number of threads\n\
	 * 		-f <path/to/file> : short reads file (FASTA/FASTQ)\n\
	 * 		-i <path/to/file> : index file\n\
	 * 		-h : help\n\
	 * 		-o <path/to/file> : output file\n\
	 * 		-r <uint32> : number of FASTA/Q records to get at once (default 100)\n\
	 * Notes:
	 * 	This program must be run to build the index before the anaylsis can be performed
	 */
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-p")==0)
		{
			num_threads = (uint16_t)atoi(argv[i + 1]);
		}	
		if(strcmp(argv[i], "-f")==0)
		{
			strcpy(reads_filename, argv[i+1]);
		}
		if(strcmp(argv[i], "-i")==0)
		{
			strcpy(index_filename, argv[i+1]);
		}
		if(strcmp(argv[i], "-o")==0)
		{
			strcpy(output_filename,argv[i+1]);
		}
		if(strcmp(argv[i], "-h")==0)
		{
			printHelp();
			return 0;
		}
		if(strcmp(argv[i], "-r")==0)
		{
			records = (uint32_t)atoi(argv[i + 1]);
		}
		
	}
	
	if(!records){err &= 1;}
	if(strcmp(index_filename,"")==0){err &= 2;}
       	if(strcmp(output_filename,"") ==0){err &= 4;}
       	if(strcmp(reads_filename,"") ==0){err &= 8;}
	
	if(err)
	{
		if(err | 1)
			std::cerr << "Invalid record block size " << records << std::endl;
		if(err | 2)
			std::cerr << "Invalid index filename " << index_filename << std::endl;
		if(err | 4)
			std::cerr << "Invalid output filename " << output_filename << std::endl;
		if(err | 8)
			std::cerr << "Invalid reads filename " << reads_filename << std::endl;
		return err;
	}
	
	IndexFileData idxfiledata = readIndexFile(index_filename);
	writeCountsFile(output_filename, countUniqueKmersInReads(reads_filename, idxfiledata.k, num_threads, idxfiledata.uniques_hash, records));

	return 0;
}
