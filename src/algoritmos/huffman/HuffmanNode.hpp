#ifndef ALGORITMOS_HUFFMAN_HUFFMAN_NODE_H
#define ALGORITMOS_HUFFMAN_HUFFMAN_NODE_H

#include <memory>
#include <cstdint>

namespace algoritmos::huffman {

class HuffmanNode {

public:
	// Folha: guarda um símbolo e quantas vezes ele aparece
	HuffmanNode(std::uint32_t symbol,std::uint64_t frequency);

	// Nó interno: recebe os dois filhos
	HuffmanNode(std::unique_ptr<HuffmanNode> leftChild, std::unique_ptr<HuffmanNode> rightChild);

	bool isLeaf() const { return left == nullptr; }
	std::uint64_t getFrequency() const { return frequency; };
	std::uint32_t getSymbol() const { return symbol; };
	const HuffmanNode* getLeft() const {return left.get(); };
	const HuffmanNode* getRight() const {return right.get(); };

private:
	std::uint64_t frequency;
	std::uint32_t symbol;
	std::unique_ptr<HuffmanNode> left;
	std::unique_ptr<HuffmanNode> right;

};

}  // namespace algoritmos::huffman
#endif
