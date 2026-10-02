#ifndef IO_BIT_WRITER_H
#define IO_BIT_WRITER_H

#include <cstdint>
#include <iosfwd>
#include <vector>

namespace io {
class BitWriter {

private:

	std::ostream& out;
	std::uint8_t buffer;
	int bitCount;


public:

	explicit BitWriter(std::ostream& out);

	void writeBit(bool bit);

	void writeCode(const std::vector<bool>& code);

	void flush();

};

}  // namespace io
#endif
