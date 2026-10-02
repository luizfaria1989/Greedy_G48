#ifndef IO_BIT_WRITER_H
#define IO_BIT_WRITER_H

#include <string>
#include <vector>

namespace io
{
class BitWriter
{
private:
	uint8_t buffer;

	int bitCount;

	std::ostream& out;


public:
	BitWriter(std::ostream& out);

	void writeBit(bool bit);

	void writeCode(std::vector<bool> code);

	void flush();

};

}  // namespace io
#endif
