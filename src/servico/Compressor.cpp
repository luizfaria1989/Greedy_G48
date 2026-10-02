#include <string>
#include <vector>
#include <list>

#include "Compressor.hpp"

namespace servico
{

Compressor::Compressor(algoritmos::predicao::Predictor* predictor, algoritmos::extracao::SymbolExtractor* extractor)
{
}

void Compressor::compress(std::string inPath, std::string outPath)
{
}

void Compressor::decompress(std::string inPath, std::string outPath)
{
}

HeaderInfo Compressor::readHeaderInfo(std::string path)
{
	return 0;
}

void Compressor::writeHeader(std::ostream& out, HeaderInfo info)
{
}

HeaderInfo Compressor::readHeader(std::istream& in)
{
	return 0;
}
}  // namespace servico
