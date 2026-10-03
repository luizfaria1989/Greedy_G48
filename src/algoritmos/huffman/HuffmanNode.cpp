#include "HuffmanNode.hpp"

#include <utility>

namespace algoritmos::huffman {

	HuffmanNode::HuffmanNode(std::uint32_t symbol, std::uint64_t frequency) : frequency(frequency),
	symbol(symbol){

	}

	HuffmanNode::HuffmanNode(std::unique_ptr<HuffmanNode> leftChild, std::unique_ptr<HuffmanNode> rightChild) :
	frequency(0),
	symbol(0),
	left(std::move(leftChild)),
	right(std::move(rightChild)) {
		frequency = left->frequency + right->frequency;
	}

}
