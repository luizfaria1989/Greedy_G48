#ifndef ALGORITMOS_HUFFMAN_HUFFMAN_TREE_H
#define ALGORITMOS_HUFFMAN_HUFFMAN_TREE_H

#include "HuffmanNode.hpp"
#include "FrequencyTable.hpp"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace io {
	class BitReader;
}

namespace algoritmos::huffman {

class HuffmanTree {

public:
	explicit HuffmanTree(const FrequencyTable& freq);

	const std::vector<bool>& getCode(std::uint32_t symbol) const;

	std::uint32_t decodeSymbol(io::BitReader& reader) const;

private:
	std::unique_ptr<HuffmanNode> root;

	std::unordered_map<std::uint32_t,std::vector<bool>> codes;

	void build(const FrequencyTable& freq);

	void generateCodes(const HuffmanNode* node, std::vector<bool> prefix);

};

}  // namespace algoritmos::huffman
#endif
