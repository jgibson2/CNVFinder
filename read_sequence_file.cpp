#include "read_sequence_file.h"

SequenceFileReader::SequenceFileReader(char* _filename) try : fileReader(toCString(getAbsolutePath(_filename)))
{
	filename = getAbsolutePath(_filename);
}catch(const std::exception& e)
{
	std::cerr << "Could not open file: " << _filename << std::endl;
}

std::string SequenceFileReader::getRecordBlock(uint16_t num_records)
{
	try
	{
		readRecords(ids, seqs, fileReader, num_records);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Could not read " << num_records << " records" << std::endl;
	}
	std::string rec_block = concat(seqs);
	seqs.clear();
	ids.clear();
	return rec_block;
}
