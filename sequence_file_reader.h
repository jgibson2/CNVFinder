#include <seqan/seq_io.h>
#include <string>
#include <stdint.h>
#include <seqan/file.h>

#define MAX_BUF_SIZE 300000000

class SequenceFileReader
{
	private:
		seqan::CharString filename;
		seqan::StringSet<seqan::CharString> ids;
		seqan::StringSet<seqan::CharString> seqs;
		seqan::SeqFileIn fileReader;
		std::string buf;
	public:
		SequenceFileReader(char*);
		std::string getRecordBlock(uint64_t, uint8_t);
		std::string getRecords(uint32_t);
		std::string getBuffer();
		bool atEnd();
};
