#include <seqan/seq_io.h>
#include <string>
#include <stdint.h>
#include <seqan/file.h>
class SequenceFileReader
{
	private:
		seqan::CharString filename;
		seqan::StringSet<seqan::CharString> ids;
		seqan::StringSet<seqan::CharString> seqs;
		seqan::SeqFileIn fileReader;
	public:
		SequenceFileReader(char*);
		std::string getRecordBlock(uint32_t);
		bool atEnd();
};
