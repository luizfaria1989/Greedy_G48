#ifndef IO_BIT_READER_H
#define IO_BIT_READER_H

#include <cstdint>
#include <iosfwd>

namespace io {

class BitReader {

private:

	std::istream& in;
	std::uint8_t buffer;
	int bitCount;



public:

	explicit BitReader(std::istream& in);

	bool readBit();

	void alignToByte();

};

}  // namespace io
#endif
