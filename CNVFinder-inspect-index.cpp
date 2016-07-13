#include <iostream>
#include "dna_hasher.h"
#include <fstream>

int main(int argc, char **argv)
{
	uint8_t k;
	if(argc != 2)
	{
		std::cerr << "Argument (index filename) required." << std::endl;
		return 1;
	}
	try
	{
		std::ifstream index_file(argv[1], std::ios::in|std::ios::binary);
		index_file.read(reinterpret_cast<char *>(&k), 1);
		std::cout << "Kmer\tLocation\n";
		while(!index_file.eof())
		{
			uint64_t kmer, location;
			index_file.read(reinterpret_cast<char*>(&kmer), 8);
			index_file.read(reinterpret_cast<char*>(&location), 8);
			std::cout << hashDNAnumtostring(kmer, k) << "\t" << location << "\n";
		}
		index_file.close();
		return 0;
	}
	catch(std::ios_base::failure& e)
	{
		std::cerr << "Could not read from file " << argv[1] << ". " << std::endl;
		return 2;
	}
}
