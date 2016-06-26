void kmer_hash_init(kmer_hash &hash, uint16_t k)
{
	hash.k = k;
	hash.hash_mutex = PTHREAD_MUTEX_INITIALIZER;
}

void* count_genomic_kmers(void* data)
{
	//cast void ptr to kmer_sequence_data type
	kmer_sequence_data seq_data = (kmer_sequence_data)data;
	pthread_mutex_lock(seq_data->hash.hash_mutex);
	
	for(uint64_t index = 0; index < seq_data.sequence.length() - seq_data.k + 1; index++)
	{
		std::string mer = seq_data.sequence.substr(index, index + seq_data.k);
		if(seq_data->hash.count(mer))
		{
			seq_data->hash.at(mer).count++;
			seq_data->hash.at(mer).positions.push_back(seq_data.start_coordinate + index);
		}
		else
		{
			seq_data->hash.emplace(mer, genomic_kmer(seq_data.k, seq_data.start_coordinate + index));
		}
