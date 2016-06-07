#ifndef __KMER_RECORD_H__
#define __KMER_RECORD_H__

#include <stdint.h>

struct kmer
{
	uint8_t k;
	uint64_t sequence;
} 

struct unique_kmer : public kmer
{
	bool isUnique;
}

struct counting_kmer: public kmer
{
	uint64_t count;	
}
#endif // __KMER_RECORD_H_
