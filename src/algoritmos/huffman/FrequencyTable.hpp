#ifndef ALGORITMOS_HUFFMAN_FREQUENCY_TABLE_H
#define ALGORITMOS_HUFFMAN_FREQUENCY_TABLE_H

#include <list>

#include "SymbolStream.hpp"
#include "unordered_map.hpp"
#include "03_Modelo/ostream.hpp"
#include "03_Modelo/istream.hpp"

namespace algoritmos
{
namespace huffman
{
class FrequencyTable
{
private:
	std::unordered_map<uint32_t,uint64_t> counts;


private:
	FrequencyTable(std::unordered_map<uint32_t,uint64_t> counts);

public:
	FrequencyTable(extracao::SymbolStream stream);

	uint64_t getFrequency(uint32_t symbol);

	std::unordered_map<uint32_t,uint64_t> getCounts();

	void writeTo(ostream& out);

	static FrequencyTable readFrom(istream& in);

	uint64_t getTotal();

};

}  // namespace huffman
}  // namespace algoritmos
#endif
