#ifndef ALGORITMOS_EXTRACAO_SYMBOL_EXTRACTOR_H
#define ALGORITMOS_EXTRACAO_SYMBOL_EXTRACTOR_H

#include <string>
#include <vector>

#include "SymbolStream.hpp"
#include "modelo/Image.hpp"

namespace algoritmos::extracao
{
class SymbolExtractor {

public:
	virtual ~SymbolExtractor() = default;
	virtual std::vector<SymbolStream> extract(const modelo::Image& img) const = 0;
	virtual modelo::Image rebuild(const std::vector<SymbolStream>& streams, int width, int height) const = 0;
	virtual std::string getName() const = 0;
};

}  // namespace algoritmos::extracao
#endif
