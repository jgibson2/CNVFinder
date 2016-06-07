#include <dna_hasher.h>

uint64_t hashDNAstringtonum(char* dna, const uint32_t size)
{
	uint64_t result = 0;
	for(int i =0; i<size; i++)
	{
		result = result << 2;
		if(dna[i] == 'C')
		{
			result |= 1;
		}
		else if(dna[i] == 'G')
		{
			result |= 2;
		}
		else if(dna[i] == 'T')
		{
			result |= 3;
		}
	}
	return result;

}

char* hashDNAnumtostring(uint64_t dna_num, const uint32_t length)
{
	const char lookuptable[4] = {'A','C','G','T'};
	char* dna = malloc(sizeof(char)*length);
	for(int i = length-1; i>=0; i--)
	{
		dna[i]=lookuptable[dna_num & 0b11];
		dna_num = dna_num >> 2;
	}
	return dna;
}
