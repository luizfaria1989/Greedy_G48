#ifndef IO_BIT_READER_H
#define IO_BIT_READER_H

#include <string>

namespace io
{
class BitReader
{
private:
	uint8_t buffer;

	int bitCount;

	std::istream& in;


public:
	BitReader(std::istream& in);

	bool readBit();

	void alignToByte();

};

}  // namespace io
#endif
