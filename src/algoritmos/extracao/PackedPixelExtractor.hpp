#ifndef ALGORITMOS_EXTRACAO_PACKED_PIXEL_EXTRACTOR_H
#define ALGORITMOS_EXTRACAO_PACKED_PIXEL_EXTRACTOR_H

#include <string>
#include <vector>

#include "SymbolExtractor.hpp"

namespace algoritmos::extracao {

	class PackedPixelExtractor : public SymbolExtractor {

	public:
		std::vector<SymbolStream> extract(const modelo::Image& img) const override;
		modelo::Image rebuild(const std::vector<SymbolStream>& streams, int width, int height) const override;
		std::string getName() const override;
	};

}  // namespace algoritmos::extracao
#endif