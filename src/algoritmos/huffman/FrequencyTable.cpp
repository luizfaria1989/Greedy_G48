#include <string>
#include <vector>
#include <list>
#include <assert.h>

#include "FrequencyTable.hpp"

namespace algoritmos
{
namespace huffman
{

FrequencyTable::FrequencyTable(algoritmos::extracao::SymbolStream stream)
{
}

uint64_t FrequencyTable::getFrequency(uint32_t symbol)
{
	return 0;
}

std::unordered_map<uint32_t,uint64_t> FrequencyTable::getCounts()
{
	return 0;
}

void FrequencyTable::writeTo(ostream& out)
{
}

FrequencyTable FrequencyTable::readFrom(istream& in)
{
	return 0;
}

uint64_t FrequencyTable::getTotal()
{
	return 0;
}

FrequencyTable::FrequencyTable(std::unordered_map<uint32_t,uint64_t> counts)
{
}
}  // namespace huffman
}  // namespace algoritmos
