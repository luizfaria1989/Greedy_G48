#include <list>

#include "HuffmanNode.hpp"

namespace algoritmos
{
namespace huffman
{

bool HuffmanNode::isLeaf()
{
	return false;
}

HuffmanNode::HuffmanNode(uint32_t symbol, uint64_t frequency)
{
}

HuffmanNode::HuffmanNode(std::unique_ptr<HuffmanNode> left, std::unique_ptr<HuffmanNode> right)
{
}
}  // namespace huffman
}  // namespace algoritmos
