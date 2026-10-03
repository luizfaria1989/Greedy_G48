#include "FrequencyTable.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace algoritmos::huffman {

	namespace {
		template<typename T>
		void writeRaw(std::ostream& out, T value) {
			out.write(reinterpret_cast<const char*>(&value),sizeof(value));
		}

		template<typename T>
		T readRaw(std::istream& in) {
			T value = 0;
			in.read(reinterpret_cast<char*>(&value), sizeof(value));
			if (!in) {
				throw std::runtime_error("Error truncated table or corrupted");
			}
			return value;
		}
	}

FrequencyTable::FrequencyTable(const extracao::SymbolStream& stream) {
	for (std::uint32_t symbol : stream) {
		counts[symbol]++;
	}
}

	FrequencyTable::FrequencyTable(Counts counts)
		: counts(std::move(counts)) {
	}

std::uint64_t FrequencyTable::getFrequency(std::uint32_t symbol) const {
		auto it = counts.find(symbol);
		if (it == counts.end()) {
			return 0;
		}
	return it->second;
}

	std::uint64_t FrequencyTable::getTotal() const {
		std::uint64_t total = 0;
		for (const auto& entry : counts) {
			total += entry.second;
		}
		return total;
	}

	void FrequencyTable::writeTo(std::ostream& out) const {
		writeRaw<std::uint32_t>(out, static_cast<std::uint32_t>(counts.size()));
		for (const auto& entry : counts) {
			writeRaw<std::uint32_t>(out, entry.first);
			writeRaw<std::uint64_t>(out, entry.second);
		}
		if (!out) {
			throw std::runtime_error("Frequency Table: failed to write on the table");
		}
	}

	FrequencyTable FrequencyTable::readFrom(std::istream& in) {
		std::uint32_t size = readRaw<std::uint32_t>(in);

		Counts loaded;
		for (std::uint32_t i = 0; i < size; i++) {
			std::uint32_t symbol = readRaw<std::uint32_t>(in);
			std::uint64_t frequency = readRaw<std::uint64_t>(in);
			loaded[symbol] = frequency;
		}
		return FrequencyTable(std::move(loaded));
	}

}  // namespace algoritmos::huffman
