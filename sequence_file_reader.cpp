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
	buf.reserve(MAX_BUF_SIZE);
}
std::string SequenceFileReader::getRecords(uint32_t num_records)
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
std::string SequenceFileReader::getRecordBlock(uint64_t max_bytes, uint8_t overhang)
{
	std::string result;
	if(buf.empty())
	{
		seqan::CharString id;
		seqan::CharString seq;
		try
		{
			seqan::readRecord(id,seq,fileReader);
		}
		catch(const std::exception& e)
		{
			std::cerr << "Could not read record" << std::endl;
		}
		buf.append(seqan::toCString(seq));
	}
	try
	{

		result = buf.substr(0,max_bytes);
		buf.erase(0,max_bytes-overhang); //take up to max_bytes - overhang off the front  of the string
	}
	catch(std::out_of_range& err)
	{
		buf.clear(); // just in case the string magically becomes empty
	}
	return result;
}
std::string SequenceFileReader::getBuffer()
{
	return buf;
}

bool SequenceFileReader::atEnd()
{
	return seqan::atEnd(fileReader)&buf.empty();
}
