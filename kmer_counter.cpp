#include "kmer_counter.h"

void* count_genomic_kmers(void* data)
{
	//cast void ptr to kmer_sequence_data type
	kmer_sequence_data<genomic_kmer> *seq_data = (kmer_sequence_data<genomic_kmer>*)data;
	pthread_mutex_lock(&(seq_data->hash->hash_mutex));
	
	for(uint64_t index = 0; index < seq_data->sequence.length() - seq_data->k + 1; index++)
	{
		std::string mer = seq_data->sequence.substr(index,seq_data->k);
		if(seq_data->hash->hash.count(mer))
		{
			seq_data->hash->hash.at(mer).isUnique = false;
		}
		else
		{
			seq_data->hash->hash.emplace(mer, genomic_kmer(1, seq_data->start_coordinate + index));
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
		std::string mer = seq_data->sequence.substr(index, seq_data->k);
		std::string rc_mer = dnaReverseComplement(seq_data->sequence.substr(index,seq_data->k));
		if(seq_data->hash->hash.count(mer))
		{
			seq_data->hash->hash.at(mer).count++;
		}
		else
		{
			seq_data->hash->hash.emplace(mer, kmer(1));
		}

		if(seq_data->hash->hash.count(rc_mer))
		{
			seq_data->hash->hash.at(rc_mer).count++;
		}
		else
		{
			seq_data->hash->hash.emplace(rc_mer, kmer(1));
		}
	
	}
	pthread_mutex_unlock(&(seq_data->hash->hash_mutex));
	pthread_exit(NULL);
}
