
class CounterThread
{
	kmer_sequence_data data;
	kmer_hash hash;
	pthread_t thread;
	pthread_mutex_t mutex;
	std::function func<void*(void*)>;
