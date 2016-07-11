#include "pthread_kmer_counter.h"

void* count_genomic_kmers(void* data)
{
	//cast void ptr to kmer_sequence_data type
	kmer_sequence_data<genomic_kmer> *seq_data = (kmer_sequence_data<genomic_kmer>*)data;
	pthread_mutex_lock(&(seq_data->hash->hash_mutex));
	
	//for(uint64_t index = 0; index < seq_data->sequence.length() - seq_data->k + 1; index++)
	for(uint64_t index = 0; index < seq_data->sequence.length() - seq_data->k; index++)
	{
		std::string mer;
		try
		{
			mer = seq_data->sequence.substr(index,seq_data->k);
			for(auto &c : mer) {c=toupper(c);}
		}
		catch(std::out_of_range& e)
		{
			break;
		}
		if(seq_data->hash->hash_map.count(mer)) //check if mer is in hash already
		{
			seq_data->hash->hash_map.at(mer).isUnique = false;
		}
		else //otherwise, construct it
		{
			seq_data->hash->hash_map.emplace(mer, genomic_kmer(1, seq_data->start_coordinate + index));
		}
	
	}
	pthread_mutex_unlock(&(seq_data->hash->hash_mutex));
	pthread_exit(NULL);
}

void* count_read_kmers(void* data)
{
	//cast void ptr to kmer_sequence_data type
	kmer_sequence_data<kmer> *seq_data = (kmer_sequence_data<kmer>*)data;
	pthread_mutex_lock(&(seq_data->hash->hash_mutex));
	
	for(uint64_t index = 0; index < seq_data->sequence.length() - seq_data->k + 1; index++)
	{
		std::string mer, rc_mer;
		try
		{
			// do the same as in genomic counter, but with reverse complement as well
			mer = seq_data->sequence.substr(index, seq_data->k);
			for(auto &c : mer) {c=toupper(c);}
			rc_mer = dnaReverseComplement(seq_data->sequence.substr(index,seq_data->k)); //upper casing taken care of in RC algorithm
		}
		catch(std::out_of_range& e)
		{
			break;
		}
		//now actually count them
		if(seq_data->hash->hash_map.count(mer))
		{
			seq_data->hash->hash_map.at(mer).count++;
		}
		else
		{
			seq_data->hash->hash_map.emplace(mer, kmer(1));
		}

		if(seq_data->hash->hash_map.count(rc_mer))
		{
			seq_data->hash->hash_map.at(rc_mer).count++;
		}
		else
		{
			seq_data->hash->hash_map.emplace(rc_mer, kmer(1));
		}
	
	}
	pthread_mutex_unlock(&(seq_data->hash->hash_mutex));
	pthread_exit(NULL);
}
