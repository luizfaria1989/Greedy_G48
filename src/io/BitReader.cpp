#include "BitReader.hpp"

#include <istream>
#include <stdexcept>
#include <cstdio>

namespace io {

BitReader::BitReader(std::istream& in)
	: in(in),
	buffer(0),
	bitCount(0) {

}

bool BitReader::readBit() {

	if (bitCount == 0) {
		int temp = in.get();
		if (temp == EOF) {
			throw std::runtime_error("BitReader::readBit: EOF");
		}

		buffer = static_cast<std::uint8_t>(temp);
		bitCount = 8;

	}

	bool bit = (buffer & 0x80) != 0;

	buffer <<= 1;
	bitCount--;

	return bit;
}

void BitReader::alignToByte() {

	buffer = 0;
	bitCount = 0;


}
}  // namespace io
