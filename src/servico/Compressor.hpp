#ifndef SERVICO_COMPRESSOR_H
#define SERVICO_COMPRESSOR_H

#include "servico/HeaderInfo.hpp"
#include "algoritmos/extracao/SymbolExtractor.hpp"
#include "algoritmos/predicao/Predictor.hpp"

#include <string>
#include <iosfwd>


namespace servico {

class Compressor {

public:
	Compressor(algoritmos::predicao::Predictor* predictor, algoritmos::extracao::SymbolExtractor* extractor);

	void compress(const std::string& inPath, const std::string& outPath) const;

	void decompress(const std::string& inPath, const std::string& outPath) const;

	static HeaderInfo readHeaderInfo(const std::string& path);

private:
	void writeHeader(std::ostream& out, const HeaderInfo& info) const;

	static HeaderInfo readHeader(std::istream& in);

	algoritmos::predicao::Predictor* predictor;
	algoritmos::extracao::SymbolExtractor* extractor;

};

}  // namespace servico
#endif
