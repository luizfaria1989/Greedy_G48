#ifndef ALGORITMOS_HUFFMAN_FREQUENCY_TABLE_H
#define ALGORITMOS_HUFFMAN_FREQUENCY_TABLE_H

#include <cstdint>
#include <iosfwd>
#include <unordered_map>

#include "algoritmos/extracao/SymbolStream.hpp"

namespace algoritmos::huffman {

class FrequencyTable {

public:

	using Counts = std::unordered_map<std::uint32_t,std::uint64_t>;

	explicit FrequencyTable(const extracao::SymbolStream& stream);

	std::uint64_t getFrequency(std::uint32_t symbol) const;
	const Counts& getCounts() const;
	std::uint64_t getTotal() const;

	void writeTo(std::ostream& out) const;
	static FrequencyTable readFrom(std::istream& in);



private:
	FrequencyTable(Counts counts);
	Counts counts;

};

}  // namespace algoritmos::huffman
#endif
