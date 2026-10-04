#ifndef SERVICO_HEADER_INFO_H
#define SERVICO_HEADER_INFO_H

#include <string>

namespace servico {

struct HeaderInfo {
	int width;
	int height;
	int numStreams;
	std::string predictorName;
	std::string extractorName;
};

}  // namespace servico
#endif
