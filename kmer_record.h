#ifndef __KMER_RECORD_H__
#define __KMER_RECORD_H__

#include <stdint.h>
#include <stdlib.h>

struct kmer
{
	uint64_t count;
	kmer(uint64_t _init_count)
	{
		count = _init_count;
	}
}; 

struct genomic_kmer : public kmer
{
	bool isUnique = true;
	uint64_t firstPosition;
	genomic_kmer(uint64_t _init_count, uint64_t _init_position) : kmer(_init_count)
	{
		firstPosition = _init_position;
	}
};

#endif // __KMER_RECORD_H_
