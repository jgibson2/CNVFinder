#include "sequence_file_reader.h"

//SequenceFileReader::SequenceFileReader(char* _filename) : fileReader(seqan::toCString(seqan::getAbsolutePath(_filename)))
SequenceFileReader::SequenceFileReader(char* _filename) : fileReader(seqan::toCString(_filename))
{
	try
	{
		filename = seqan::getAbsolutePath(_filename);
	}
	
	catch(const std::exception& e)
	{
		std::cerr << "Could not open file: " << _filename << std::endl;
	}
	
}
std::string SequenceFileReader::getRecordBlock(uint32_t num_records)
{
	try
	{
		seqan::readRecords(ids, seqs, fileReader, num_records);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Could not read " << num_records << " records" << std::endl;
	}
	std::string rec_block;
	for(auto seq: seqs)
	{
		rec_block.append(seqan::toCString(seq));
	}
	seqan::clear(seqs);
	seqan::clear(ids);
	return rec_block;
}

bool SequenceFileReader::atEnd()
{
	return seqan::atEnd(fileReader);
}
