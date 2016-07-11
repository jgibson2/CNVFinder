#include "dna_hasher.h"

uint64_t hashDNAstringtonum(std::string dna)
{
	uint64_t result = 0;
	for(std::string::iterator it = dna.begin(); it != dna.end(); ++it)
	{
		result = result << 2;
		switch(*it)
		{
			case 'c':
			case 'C':
				result |= 1;
				break;
			case 'g':
			case 'G':
				result |= 2;
				break;
			case 't':
			case 'T':
				result |= 3;
				break;
			default:
				break;
		}
	}
	return result;

}

std::string hashDNAnumtostring(uint64_t dna_num, const uint8_t length)
{
	const char lookuptable[4] = {'A','C','G','T'};
	std::string dna = "";
	for(uint8_t i = length; i > 0; i--)
	{
		dna.push_back(lookuptable[dna_num & 3]);
		dna_num = dna_num >> 2;
	}
	std::reverse(dna.begin(), dna.end());
	return dna;
}

std::string dnaReverseComplement(std::string dna)
{
	auto lambda = [](const char c)
	{
		switch(c)
		{
			case 'a':
			case 'A': 
				return 'T';
			case 'g':
			case 'G':
				return 'C';
			case 'c':
			case 'C':
				return 'G';
			case 't':
			case 'T':
				return 	'A';
			default:
				return 'N'; //gracefully return N for other bases
		}
	};
	std::transform(dna.cbegin(), dna.cend(), dna.begin(), lambda);
	std::reverse(dna.begin(), dna.end());
	return dna;
}
