#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>

#include "glm/glm.hpp"
#include "args.hpp"

#define VERSION "0.2"

using namespace glm;

enum Commands {
	NONE = 0,
	SCALE = 1,
	CENTERX = 2,
	CENTERY = 4,
	CENTERZ = 8
};

int main(int argc, char* argv[]) {
	Args args(argc, argv);
	if (args.opt('v', "version")) {
		std::cerr << VERSION << std::endl;
		return EXIT_SUCCESS;
	}
	if (args.opt('h', "help") || argc < 3) {
		std::cerr << "Usage: " << args.app() << " PARAM [PARAM...] FILE" << std::endl;
		std::cerr << "Parameters:" << std::endl;
		std::cerr << " -h   --help            print this help and exit" << std::endl;
		std::cerr << " -v   --version         print version and exit" << std::endl;
		std::cerr << " -s   --scale SCALE     scale object along all axes SCALE amount" << std::endl;
		std::cerr << " -c   --center          center object along all axes" << std::endl;
		std::cerr << "      --centerx         center object x axis" << std::endl;
		std::cerr << "      --centery         center object y axis" << std::endl;
		std::cerr << "      --centerz         center object z axis" << std::endl;
		std::cerr << "Example: " << args.app() << " --scale 0.5 model.obj" << std::endl;
		return args.opt('h', "help") ? EXIT_SUCCESS : EXIT_FAILURE;
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
	vec3 lbound(std::numeric_limits<float>::max());
	vec3 ubound(-std::numeric_limits<float>::max());
	while (getline(file, row)) {
		std::istringstream srow(row);
		vec3 in;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> in.x >> in.y >> in.z;
			lbound = min(in, lbound);
			ubound = max(in, ubound);
		}
	}
	vec3 center = (lbound + ubound) * 0.5f;

	// Output pass
	file.clear();
	file.seekg(0, std::ios::beg);
	while (getline(file, row)) {
		std::istringstream srow(row);
		vec3 in;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> in.x >> in.y >> in.z;
			if (mode & CENTERX) in.x -= center.x;
			if (mode & CENTERY) in.y -= center.y;
			if (mode & CENTERZ) in.z -= center.z;
			if (mode & SCALE) in *= scale;
			std::cout << "v " << in.x << " " << in.y << " " << in.z << std::endl;
		} else {
			std::cout << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
