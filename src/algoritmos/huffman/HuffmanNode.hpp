#ifndef LGORITMOS_HUFFMAN_HUFFMAN_NODE_H
#define ALGORITMOS_HUFFMAN_HUFFMAN_NODE_H

#include <string>

namespace algoritmos
{
namespace huffman
{
class HuffmanNode
{
private:
	int64_t frequency;

	uint32_t symbol;

	std::unique_ptr<HuffmanNode> left;

	std::unique_ptr<HuffmanNode> right;


public:
	bool isLeaf();

	HuffmanNode(uint32_t symbol,uint64_t frequency);

	HuffmanNode(std::unique_ptr<HuffmanNode> left, std::unique_ptr<HuffmanNode> right);

};

}  // namespace huffman
}  // namespace algoritmos
#endif
