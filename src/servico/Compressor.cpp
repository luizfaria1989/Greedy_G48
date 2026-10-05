#include "Compressor.hpp"

#include "io/BitReader.hpp"
#include "io/BitWriter.hpp"
#include "io/PPMFile.hpp"
#include "algoritmos/huffman/FrequencyTable.hpp"
#include "algoritmos/huffman/HuffmanTree.hpp"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <vector>

using algoritmos::extracao::SymbolStream;
using algoritmos::huffman::FrequencyTable;
using algoritmos::huffman::HuffmanTree;

namespace {

	constexpr char MAGIC[] = "HUF1";
	constexpr std::streamsize MAGIC_SIZE = 4;

	constexpr std::uint32_t MAX_NAME_LENGTH = 64;
	constexpr std::streamsize MAX_STREAMS = 3;

	template <typename T>
	void writeRaw(std::ostream& out, T value) {
		out.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	template <typename T>
	T readRaw(std::istream& in) {
		T value = 0;
		in.read(reinterpret_cast<char*>(&value), sizeof(value));
		if (!in) {
			throw std::runtime_error("Compressor: Trucanted header");
		}
		return value;
	}

	void writeString(std::ostream& out, const std::string& text) {
		writeRaw<std::uint32_t>(out, static_cast<std::uint32_t>(text.size()));
		out.write(text.data(), static_cast<std::uint32_t>(text.size()));
	}

	std::string readString(std::istream& in) {
		std::uint32_t lenght = readRaw<std::uint32_t>(in);
		if (lenght == 0 || lenght > MAX_NAME_LENGTH) {
			throw std::runtime_error("Compressor: Inalidade name on the header");
		}
		std::string text(lenght, '\0');
		in.read(text.data(), static_cast<std::streamsize>(lenght));
		if (!in) {
			throw std::runtime_error("Compressor: Trucanted header");
		}
		return text;
	}

}

namespace servico {

Compressor::Compressor(algoritmos::predicao::Predictor* predictor, algoritmos::extracao::SymbolExtractor* extractor)
	: predictor(predictor),
	extractor(extractor) {
	if (predictor == nullptr || extractor == nullptr) {
		throw std::invalid_argument("Predictor or extractor is nullptr");
	}
}

void Compressor::compress(const std::string& inPath, const std::string& outPath) const {

	modelo::Image img = io::PPMFile::read(inPath);
	modelo::Image residuos = predictor->encode(img);
	auto streams = extractor->extract(residuos);

	HeaderInfo info {
		img.getWidth(),
		img.getHeight(),
		static_cast<int>(streams.size()),
		predictor->getName(),
		extractor->getName()
	};

	std::ofstream out(outPath, std::ios::binary);
	if (!out.is_open()) {
		throw std::runtime_error("Compressor: Failed to open output file");
	}

	io::BitWriter writer(out);
	writeHeader(out, info);

	for (const SymbolStream& stream : streams) {

		FrequencyTable freq(stream);
		freq.writeTo(out);
		HuffmanTree tree(freq);

		for (std::uint32_t symbol : stream) {
			writer.writeCode(tree.getCode(symbol));
		}
		writer.flush();
	}
	if (!out) {
		throw std::runtime_error("Compressor: Failed on writing on: " + outPath);
	}
}

void Compressor::decompress(const std::string& inPath, const std::string& outPath) const {

	std::ifstream in(inPath, std::ios::binary);
	if (!in.is_open()) {
		throw std::runtime_error("Compressor: Failed to open input: " + inPath);
	}

	HeaderInfo header = readHeader(in);

	if (header.predictorName != predictor->getName() ||
		header.extractorName != extractor->getName()) {
		throw std::invalid_argument("Compressor: o arquivo foi comprimido com " + header.predictorName + "e " + header.extractorName);
	}

	io::BitReader reader(in);

	std::vector<SymbolStream> streams;

	for (int s = 0; s < header.numStreams; ++s) {
		FrequencyTable freq = FrequencyTable::readFrom(in);
		HuffmanTree tree(freq);

		std::uint64_t total = freq.getTotal();
		SymbolStream stream;

		for (std::uint64_t i = 0; i < total; ++i) {

			stream.push_back(tree.decodeSymbol(reader));
		}
		reader.alignToByte();
		streams.push_back(std::move(stream));
	}

	modelo::Image residuos = extractor->rebuild(streams, header.width, header.height);
	modelo::Image img = predictor->decode(residuos);

	io::PPMFile::write(outPath, img);

}

HeaderInfo Compressor::readHeaderInfo(const std::string& path) {
	std::ifstream in(path, std::ios::binary);
	if (!in.is_open()) {
		throw std::runtime_error("Compressor: Failed to open input: " + path);
	}
	return readHeader(in);
}

void Compressor::writeHeader(std::ostream& out, const HeaderInfo& info) const {

	out.write(MAGIC, MAGIC_SIZE);
	writeRaw<std::uint32_t>(out, static_cast<std::uint32_t>(info.width));
	writeRaw<std::uint32_t>(out, static_cast<std::uint32_t>(info.height));
	writeRaw<std::uint32_t>(out, static_cast<std::uint32_t>(info.numStreams));
	writeString(out, info.predictorName);
	writeString(out, info.extractorName);
}

HeaderInfo Compressor::readHeader(std::istream& in) {
	char magic[MAGIC_SIZE];
	in.read(magic, MAGIC_SIZE);
	if (!in || std::string(magic, MAGIC_SIZE) != MAGIC) {
		throw std::runtime_error("Compressor: File is not a .huf");
	}

	std::uint32_t width = readRaw<std::uint32_t>(in);
	std::uint32_t height = readRaw<std::uint32_t>(in);
	std::uint32_t numStreams = readRaw<std::uint32_t>(in);

	if (width == 0 || height == 0) {
		throw std::invalid_argument("Invalid dimensions on the header");
	}

	if (numStreams == 0 || numStreams > MAX_STREAMS) {
		throw std::invalid_argument("Invalid numStreams");
	}

	std::string predictorName = readString(in);
	std::string extractorName = readString(in);

	return HeaderInfo {
		static_cast<int>(width),
		static_cast<int>(height),
		static_cast<int>(numStreams),
		predictorName,
		extractorName};
}
}  // namespace servico
