#include <dna_hasher.h>

uint64_t hashDNAstringtonum(std::string dna)
{
	uint64_t result = 0;
	for(std::string::iterator it = dna.begin(); it != dna.end(); ++it)
	{
		result = result << 2;
		if(*it == 'C')
		{
			result |= 1;
		}
		else if(*it == 'G')
		{
			result |= 2;
		}
		else if(*it == 'T')
		{
			result |= 3;
		}
	}
	return result;

}

std::string hashDNAnumtostring(uint64_t dna_num, const uint32_t length)
{
	const char lookuptable[4] = {'A','C','G','T'};
	std::string dna = "";
	for(uint32_t i = length; i > 0; i--)
	{
		dna.push_back(lookuptable[dna_num & 3])
		dna_num = dna_num >> 2;
	}
	std::reverse(dna.begin(), dna.end());
	return dna;
}
