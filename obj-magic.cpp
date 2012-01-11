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
		std::cerr << "      --scalex SCALE       scale object along x axis SCALE amount" << std::endl;
		std::cerr << "      --scaley SCALE       scale object along y axis SCALE amount" << std::endl;
		std::cerr << "      --scalez SCALE       scale object along z axis SCALE amount" << std::endl;
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

	vec3 scale(args.arg('s', "scale", 1.0f));
	scale.x *= args.arg(' ', "scalex", 1.0f);
	scale.y *= args.arg(' ', "scaley", 1.0f);
	scale.z *= args.arg(' ', "scalez", 1.0f);

	vec3 translate(args.arg(' ', "translate", 0.0f));
	translate.x += args.arg(' ', "translatex", 0.0f);
	translate.y += args.arg(' ', "translatey", 0.0f);
	translate.z += args.arg(' ', "translatez", 0.0f);

	ivec3 center;
	if (args.opt('c', "center"))  center = ivec3(1);
	if (args.opt(' ', "centerx")) center.x = 1;
	if (args.opt(' ', "centery")) center.y = 1;
	if (args.opt(' ', "centerz")) center.z = 1;

	ivec3 mirror;
	if (args.opt(' ', "mirror"))  mirror = ivec3(1);
	if (args.opt(' ', "mirrorx")) mirror.x = 1;
	if (args.opt(' ', "mirrory")) mirror.y = 1;
	if (args.opt(' ', "mirrorz")) mirror.z = 1;

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
	center *= (lbound + ubound) * 0.5f;

	// Output pass
	file.clear();
	file.seekg(0, std::ios::beg);
	while (getline(file, row)) {
		std::istringstream srow(row);
		vec3 in;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> in.x >> in.y >> in.z;
			if (center.length() > 0.0f) in -= center;
			if (mirror.length() > 0.0f) in *= mirror;
			if (scale.length() != 1.0f) in *= scale;
			if (translate.length() > 0.0f) in += translate;
			std::cout << "v " << in.x << " " << in.y << " " << in.z << std::endl;
		} else {
			std::cout << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
