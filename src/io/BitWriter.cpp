#include "BitWriter.hpp"
#include <stdexcept>

#include <ostream>

namespace io {

BitWriter::BitWriter(std::ostream& out)
    : out(out),
    buffer(0),
    bitCount(0) {

}

void BitWriter::writeBit(bool bit) {

    buffer <<= 1;
    buffer |= bit;
    bitCount++;

    if (bitCount == 8) {
        out.put(static_cast<char>(buffer));
        buffer = 0;
        bitCount = 0;
    }

}

void BitWriter::writeCode(const std::vector<bool>& code) {

    for (bool bit : code) {
        writeBit(bit);
    }

}

void BitWriter::flush() {

    if (bitCount != 0) {
        buffer <<= (8 - bitCount);
        out.put(static_cast<char>(buffer));
        buffer = 0;
        bitCount = 0;
    }

    if (!out) {
        throw std::runtime_error("BitWriter::flush: failed to write on the stream");
    }


}

}  // namespace io
