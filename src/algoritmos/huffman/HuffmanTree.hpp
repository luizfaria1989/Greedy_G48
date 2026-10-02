#ifndef ALGORITMOS_HUFFMAN_HUFFMAN_TREE_H
#define ALGORITMOS_HUFFMAN_HUFFMAN_TREE_H

#include <string>
#include <vector>
#include <list>

#include "algoritmos/huffman/HuffmanNode.hpp"
#include "algoritmos/huffman/FrequencyTable.hpp"
#include "io/BitReader.hpp"

namespace algoritmos
{
namespace huffman
{
class HuffmanTree
{
private:
	std::unique_ptr<HuffmanNode> root;

	std::unordered_map<uint32_t,std::vector<bool>> codes;


private:
	void build(FrequencyTable freq);

	void generateCodes(HuffmanNode* node, std::vector<bool> prefix);

public:
	HuffmanTree(FrequencyTable freq);

	std::vector<bool> getCode(uint32_t symbol);

	uint32_t decodeSymbol(io::BitReader& reader);

};

}  // namespace huffman
}  // namespace algoritmos
#endif
