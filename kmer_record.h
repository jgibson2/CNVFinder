#ifndef __KMER_RECORD_H__
#define __KMER_RECORD_H__

#include <stdint.h>
#include <stdlib.h>

struct kmer
{
	uint64_t firstPosition;
	kmer(uint64_t _init_position) : firstPosition(_init_position)
	{
	}
};

struct genomic_kmer : public kmer
{
	bool isUnique = true;
	genomic_kmer(uint64_t _init_position) : kmer(_init_position)
	{
	}
};

struct read_kmer : public kmer
{
	uint64_t count;
	read_kmer(uint64_t _init_position, uint64_t _init_count): kmer(_init_position),count(_init_count)
	{
	}
};

#endif // __KMER_RECORD_H_
