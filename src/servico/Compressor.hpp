#ifndef SERVICO_COMPRESSOR_H
#define SERVICO_COMPRESSOR_H

#include <string>

#include "servico/HeaderInfo.hpp"
#include "../algoritmos/extracao/SymbolExtractor.hpp"
#include "../algoritmos/predicao/Predictor.hpp"

namespace servico
{
class Compressor
{
private:
	void writeHeader(std::ostream& out, HeaderInfo info);

	static HeaderInfo readHeader(std::istream& in);

public:
	Compressor(algoritmos::predicao::Predictor* predictor, algoritmos::extracao::SymbolExtractor* extractor);

	void compress(std::string inPath, std::string outPath);

	void decompress(std::string inPath, std::string outPath);

	static HeaderInfo readHeaderInfo(std::string path);

};

}  // namespace servico
#endif
