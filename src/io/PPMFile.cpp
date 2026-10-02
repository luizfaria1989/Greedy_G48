#include "PPMFile.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <limits>

namespace io{

modelo::Image PPMFile::read(const std::string& path) {

	std::ifstream in(path, std::ios::binary);

	if (!in.is_open()) {
		throw std::runtime_error("Unable to open file: " + path);
	}

	std::string magic;
	in >> magic;

	if (magic != "P6") {
		throw std::runtime_error("Unsupported file format: " + magic);
	}

	int width = readHeaderValue(in);
	int height = readHeaderValue(in);
	int maxValue = readHeaderValue(in);

	if (width <= 0) {
		throw std::runtime_error("Invalid PPM width: " + std::to_string(width));
	}

	if (height <= 0) {
		throw std::runtime_error("Invalid PPM height: " + std::to_string(height));
	}

	if (maxValue != 255) {
		throw std::runtime_error("Invalid PPM maxValue: " + std::to_string(maxValue));
	}

	in.get();

	std::size_t total = static_cast<std::size_t>(width) * height * 3;
	std::vector<uint8_t> buffer(total);

	in.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(total));

	in.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(total));

	if (static_cast<std::size_t>(in.gcount()) != total) {
		throw std::runtime_error("Trucanded PPM File: " + path);
	}

	modelo::Image img(width, height);
	std::size_t i = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int c = 0; c < 3; c++) {
				img.setPixel(x, y, c, buffer[i]);
				i++;
			}
		}
	}
	return img;
}

void PPMFile::write(const std::string& path, const modelo::Image& img) {

	std::ofstream out(path, std::ios::binary);

	if (!out.is_open()) {
		throw std::runtime_error("Unable to create file: " + path);
	}

	int width = img.getWidth();
	int height = img.getHeight();

	out << "P6\n" << width << ' ' << height << "\n255\n";

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int c = 0; c < 3; c++) {
				out.put(static_cast<char>(img.getPixel(x, y, c)));
			}
		}
	}

	if (!out) {
		throw std::runtime_error("Error while writing file: " + path);
	}

}

int PPMFile::readHeaderValue(std::istream &in) {

	while (true) {
		in >> std::ws;
		if (in.peek() != '#') {
			break;
		}
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	int value;
	in >> value;

	if (in.fail()) {
		throw std::runtime_error("Invalid PPM header");
	}

	return value;
}
}  // namespace io
