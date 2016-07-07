#include <seqan/seq_io.h>
#include <string>
#include <stdint.h>

class SequenceFileReader
{
	private:
		CharString filename;
		StringSet<std::string> ids;
		StringSet<std::string> seqs;
		SeqFileIn fileReader;
	public:
		SequenceFileReader(char*);
		getRecordBlock(uint16_t);
}
