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
	CENTERZ = 8,
	CENTERALL = CENTERX | CENTERY | CENTERZ,
	MIRRORX = 16,
	MIRRORY = 32,
	MIRRORZ = 64,
	MIRRORALL = MIRRORX | MIRRORY | MIRRORZ,
	TRANSLATEX = 128,
	TRANSLATEY = 256,
	TRANSLATEZ = 512,
	TRANSLATEALL = TRANSLATEX | TRANSLATEY | TRANSLATEZ
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
		std::cerr << " -h   --help               print this help and exit" << std::endl;
		std::cerr << " -v   --version            print version and exit" << std::endl;
		std::cerr << " -s   --scale SCALE        scale object along all axes SCALE amount" << std::endl;
		std::cerr << " -c   --center             center object along all axes" << std::endl;
		std::cerr << "      --centerx            center object x axis" << std::endl;
		std::cerr << "      --centery            center object y axis" << std::endl;
		std::cerr << "      --centerz            center object z axis" << std::endl;
		std::cerr << "      --mirror             mirror object along all axes" << std::endl;
		std::cerr << "      --mirrorx            mirror object along x axis" << std::endl;
		std::cerr << "      --mirrory            mirror object along y axis" << std::endl;
		std::cerr << "      --mirrorz            mirror object along z axis" << std::endl;
		std::cerr << "      --translate AMOUNT   translate along all axes AMOUNT amount" << std::endl;
		std::cerr << "      --translatex AMOUNT  translate along x axis AMOUNT amount" << std::endl;
		std::cerr << "      --translatey AMOUNT  translate along y axis AMOUNT amount" << std::endl;
		std::cerr << "      --translatez AMOUNT  translate along z axis AMOUNT amount" << std::endl;
		std::cerr << "Example: " << args.app() << " --scale 0.5 model.obj" << std::endl;
		return args.opt('h', "help") ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	int mode = NONE;

	float scale = args.arg('s', "scale", -1.0f);
	if (scale > 0.0f) mode |= SCALE;

	float t = args.arg(' ', "translate", 0.0f);
	vec3 translate(t);
	if (t != 0.0f) mode |= TRANSLATEALL;
	t = args.arg(' ', "translate", 0.0f);
	if ((t = args.arg(' ', "translatex", 0.0f)) != 0.0f) { mode |= TRANSLATEX; translate.x += t; }
	if ((t = args.arg(' ', "translatey", 0.0f)) != 0.0f) { mode |= TRANSLATEY; translate.y += t; }
	if ((t = args.arg(' ', "translatez", 0.0f)) != 0.0f) { mode |= TRANSLATEZ; translate.z += t; }

	if (args.opt('c', "center"))  mode |= CENTERALL;
	if (args.opt(' ', "centerx")) mode |= CENTERX;
	if (args.opt(' ', "centery")) mode |= CENTERY;
	if (args.opt(' ', "centerz")) mode |= CENTERZ;

	if (args.opt(' ', "mirror"))  mode |= MIRRORALL;
	if (args.opt(' ', "mirrorx")) mode |= MIRRORX;
	if (args.opt(' ', "mirrory")) mode |= MIRRORY;
	if (args.opt(' ', "mirrorz")) mode |= MIRRORZ;

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
			if (mode & MIRRORX) in.x = -in.x;
			if (mode & MIRRORY) in.y = -in.y;
			if (mode & MIRRORZ) in.z = -in.z;
			if (mode & SCALE) in *= scale;
			if (mode & TRANSLATEALL) in += translate;
			std::cout << "v " << in.x << " " << in.y << " " << in.z << std::endl;
		} else {
			std::cout << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
