#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>

#include "args.hpp"

enum Commands {
	NONE = 0,
	SCALE = 1,
	CENTERX = 2,
	CENTERY = 4,
	CENTERZ = 8
};

int main(int argc, char* argv[]) {
	Args args(argc, argv);
	if (args.opt('h', "help") || argc < 3) {
		std::cerr << "Usage:    " << args.app() << " COMMAND [PARAMS] FILE" << std::endl;
		std::cerr << "Example:  " << args.app() << " --scale 0.5 model.obj" << std::endl;
		return EXIT_FAILURE;
	}
	int mode = NONE;

	float scale = args.arg('s', "scale", -1.0f);
	if (scale > 0.0f) mode |= SCALE;
	if (args.opt('c', "center")) mode |= CENTERX | CENTERY | CENTERZ;
	if (args.opt(' ', "centerx")) mode |= CENTERX;
	if (args.opt(' ', "centery")) mode |= CENTERY;
	if (args.opt(' ', "centerz")) mode |= CENTERZ;

	std::string filename = argv[argc-1];
	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file " << filename << std::endl;
		return EXIT_FAILURE;
	}

	std::string row;
	// Analyzing pass
	float xmin = std::numeric_limits<float>::max();
	float xmax = -std::numeric_limits<float>::max();
	float ymin = std::numeric_limits<float>::max();
	float ymax = -std::numeric_limits<float>::max();
	float zmin = std::numeric_limits<float>::max();
	float zmax = -std::numeric_limits<float>::max();
	while (getline(file, row)) {
		std::istringstream srow(row);
		float x,y,z;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> x >> y >> z;
			xmin = std::min(x, xmin);
			ymin = std::min(y, ymin);
			zmin = std::min(z, zmin);
			xmax = std::max(x, xmax);
			ymax = std::max(y, ymax);
			zmax = std::max(z, zmax);
		}
	}
	float cx = (xmin + xmax) * 0.5;
	float cy = (ymin + ymax) * 0.5;
	float cz = (zmin + zmax) * 0.5;

	// Output pass
	file.clear();
	file.seekg(0, std::ios::beg);
	while (getline(file, row)) {
		std::istringstream srow(row);
		float x,y,z;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> x >> y >> z;
			if (mode & CENTERX) x -= cx;
			if (mode & CENTERY) y -= cy;
			if (mode & CENTERZ) z -= cz;
			if (mode & SCALE) { x *= scale; y *= scale; z *= scale; }
			std::cout << "v " << x << " " << y << " " << z << std::endl;
		} else {
			std::cout << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
