#include "HuffmanTree.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <string>
#include "io/BitReader.hpp"

namespace {
	struct HeapEntry {
		std::uint64_t freq;
		std::uint32_t order;
		std::unique_ptr<algoritmos::huffman::HuffmanNode> node;
	};
}

namespace algoritmos::huffman {

	HuffmanTree::HuffmanTree(const FrequencyTable& freq) {

		build(freq);

		if (root == nullptr) {
			// nada
		} else if (root->isLeaf()) {
			codes[root->getSymbol()] = {false};
		} else {
			generateCodes(root.get(), {});
		}
	}

	const std::vector<bool>& HuffmanTree::getCode(std::uint32_t symbol) const {
		auto it = codes.find(symbol);
		if (it == codes.end()) {
			throw std::out_of_range("HuffmanTree::getCode: symbol without code: " + std::to_string(symbol));
		}
		return it->second;
	}

	std::uint32_t HuffmanTree::decodeSymbol(io::BitReader& reader) const{

		if (root == nullptr) {
			throw std::runtime_error("HuffmanTree::decodeSymbol: root node is null");
		} else if (root->isLeaf()) {
			reader.readBit();
			return root->getSymbol();
		} else {
			const HuffmanNode* node = root.get();
			while (!node->isLeaf()) {
				bool bit = reader.readBit();
				node = bit ? node->getRight() : node->getLeft();
			}
			return node->getSymbol();
		}
	}

	void HuffmanTree::build(const FrequencyTable& freq) {

		const auto& counts = freq.getCounts();
		std::vector<std::pair<std::uint32_t, std::uint64_t>> pairs(counts.begin(), counts.end());

		std::sort(pairs.begin(), pairs.end());

		std::vector<HeapEntry> heap;
		std::uint32_t order = 0;

		for (const auto& p : pairs) {
			std::unique_ptr<HuffmanNode> leaf = std::make_unique<HuffmanNode>(p.first, p.second);
			heap.push_back(HeapEntry {p.second, order, std::move(leaf)});
			order++;
		}

		auto cmp = [](const HeapEntry& a, const HeapEntry& b) {
			if (a.freq != b.freq) {
				return a.freq > b.freq;
			}
			return a.order > b.order;
		};

		std::make_heap(heap.begin(), heap.end(), cmp);

		while (heap.size() > 1) {
			std::pop_heap(heap.begin(), heap.end(), cmp);
			HeapEntry a = std::move(heap.back());
			heap.pop_back();

			std::pop_heap(heap.begin(), heap.end(), cmp);
			HeapEntry b = std::move(heap.back());
			heap.pop_back();

			std::uint64_t freq_sum = a.freq + b.freq;
			auto parent = std::make_unique<HuffmanNode>(std::move(a.node), std::move(b.node));

			heap.push_back(HeapEntry {freq_sum, order, std::move(parent)});
			order++;
			std::push_heap(heap.begin(), heap.end(), cmp);
		}

		if (!heap.empty()) {
			root = std::move(heap.front().node);
		}
	}

	void HuffmanTree::generateCodes(const HuffmanNode* node, std::vector<bool> prefix) {

		if (node->isLeaf()) {
			codes[node->getSymbol()] = prefix;
			return;
		} else {
			std::vector<bool> leftPrefix = prefix;
			leftPrefix.push_back(false);
			generateCodes(node->getLeft(), leftPrefix);

			prefix.push_back(true);
			generateCodes(node->getRight(), prefix);
		}
	}
}  // namespace algoritmos::huffman
