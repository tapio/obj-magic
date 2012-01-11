#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "args.hpp"

#define APPNAME "obj-magic"
#define VERSION "v0.2"

using namespace glm;

std::string toString(vec3 vec) {
	std::ostringstream oss;
	oss << "x:" << vec.x << " y:" << vec.y << " z:" << vec.z;
	return oss.str();
}

int main(int argc, char* argv[]) {
	Args args(argc, argv);
	if (args.opt('v', "version")) {
		std::cerr << APPNAME << " " << VERSION << std::endl;
		return EXIT_SUCCESS;
	}
	if (args.opt('h', "help") || argc < 3) {
		std::cerr << "Usage: " << args.app() << " PARAM [PARAM...] FILE" << std::endl;
		std::cerr << "Parameters:" << std::endl;
		std::cerr << " -h   --help               print this help and exit" << std::endl;
		std::cerr << " -v   --version            print version and exit" << std::endl;
		std::cerr << " -i   --info               print info about the object and exit" << std::endl;
		std::cerr << " -n   --normalize-normals  renormalize all normals" << std::endl;
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
		std::cerr << "      --rotate AMOUNT      rotate along all axes AMOUNT degrees" << std::endl;
		std::cerr << "      --rotatex AMOUNT     rotate along x axis AMOUNT degrees" << std::endl;
		std::cerr << "      --rotatey AMOUNT     rotate along y axis AMOUNT degrees" << std::endl;
		std::cerr << "      --rotatez AMOUNT     rotate along z axis AMOUNT degrees" << std::endl;
		std::cerr << "Example: " << args.app() << " --scale 0.5 model.obj" << std::endl;
		return args.opt('h', "help") ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	bool info = args.opt('i', "info");
	bool normalize_normals = args.opt('n', "normalize-normals");

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

	vec3 rotangles(args.arg(' ', "rotate", 0.0f));
	rotangles.x += args.arg(' ', "rotatex", 0.0f);
	rotangles.y += args.arg(' ', "rotatey", 0.0f);
	rotangles.z += args.arg(' ', "rotatez", 0.0f);
	mat4 temprot(1.0f);
	if (rotangles.x != 0.0f) temprot = rotate(temprot, rotangles.x, vec3(1,0,0));
	if (rotangles.y != 0.0f) temprot = rotate(temprot, rotangles.y, vec3(0,1,0));
	if (rotangles.z != 0.0f) temprot = rotate(temprot, rotangles.z, vec3(0,0,1));
	mat3 rotation(temprot);

	std::string filename = argv[argc-1];
	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file " << filename << std::endl;
		return EXIT_FAILURE;
	}

	std::string row;
	// Analyzing pass
	bool analyze = info || (center.length() > 0.0f);
	if (analyze) {
		vec3 lbound(std::numeric_limits<float>::max());
		vec3 ubound(-std::numeric_limits<float>::max());
		unsigned long long v_count = 0, vt_count = 0, vn_count = 0, f_count = 0;
		while (getline(file, row)) {
			std::istringstream srow(row);
			vec3 in;
			std::string tempst;
			if (row.substr(0,2) == "v ") {  // Vertices
				srow >> tempst >> in.x >> in.y >> in.z;
				lbound = min(in, lbound);
				ubound = max(in, ubound);
				++v_count;
			}
			else if (row.substr(0,3) == "vt ") ++vt_count;
			else if (row.substr(0,3) == "vn ") ++vn_count;
			else if (row.substr(0,2) == "f ") ++f_count;
		}
		center *= (lbound + ubound) * 0.5f;
		// Output info?
		if (info) {
			std::cout << APPNAME << " " << VERSION << std::endl;
			std::cout << "Filename: " << filename << std::endl;
			std::cout << std::endl;
			std::cout << "Vertices: " << v_count << std::endl;
			std::cout << "TexCoords: " << vt_count << std::endl;
			std::cout << "Normals: " << vn_count << std::endl;
			std::cout << "Faces: " << f_count << std::endl;
			std::cout << std::endl;
			std::cout << "Center: " << toString((lbound + ubound) * 0.5f) << std::endl;
			std::cout << "Size: " << toString(ubound - lbound) << std::endl;
			std::cout << "Lower bounds: " << toString(lbound) << std::endl;
			std::cout << "Upper bounds: " << toString(ubound) << std::endl;
			return EXIT_SUCCESS;
		}
	}

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
			if (rotangles.length() > 0.0f) in = rotation * in;
			if (translate.length() > 0.0f) in += translate;
			std::cout << "v " << in.x << " " << in.y << " " << in.z << std::endl;
		} else if (row.substr(0,3) == "vn ") {  // Normals
			srow >> tempst >> in.x >> in.y >> in.z;
			if (normalize_normals) normalize(in);
			std::cout << "vn " << in.x << " " << in.y << " " << in.z << std::endl;
		} else {
			std::cout << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
