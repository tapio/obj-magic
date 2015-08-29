#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "args.hpp"

#define APPNAME "obj-magic"
#define VERSION "v0.2"

#define EPSILON 0.00001f

using namespace glm;

std::string toString(vec3 vec) {
	std::ostringstream oss;
	oss << "x:" << vec.x << " y:" << vec.y << " z:" << vec.z;
	return oss.str();
}

template<typename T> inline bool isZero(T v) { v = abs(v); return v.x < EPSILON && v.y < EPSILON && v.z < EPSILON; }
template<typename T> inline bool isOne(T v) { return isZero(v - T(1)); }

int main(int argc, char* argv[]) {
	Args args(argc, argv);
	if (args.opt('v', "version")) {
		std::cerr << APPNAME << " " << VERSION << std::endl;
		return EXIT_SUCCESS;
	}
	if (args.opt('h', "help") || argc < 3) {
		std::cerr << "Usage: " << args.app() << " PARAM [PARAM...] FILE" << std::endl;
		std::cerr << "Parameters:" << std::endl;
		std::cerr << " -h   --help                    print this help and exit" << std::endl;
		std::cerr << " -v   --version                 print version and exit" << std::endl;
		std::cerr << " -o   --out FILE                put output to FILE instead of stdout" << std::endl;
		std::cerr << " -i   --info                    print info about the object and exit" << std::endl;
		std::cerr << " -n   --normalize-normals       renormalize all normals" << std::endl;
		std::cerr << " -s   --scale[xyz] SCALE        scale object SCALE amount" << std::endl;
		std::cerr << " -c   --center[xyz]             center object" << std::endl;
		std::cerr << "      --mirror[xyz]             mirror object" << std::endl;
		std::cerr << "      --translate[xyz] AMOUNT   translate AMOUNT amount" << std::endl;
		std::cerr << "      --rotate[xyz] AMOUNT      rotate along axis AMOUNT degrees" << std::endl;
		std::cerr << std::endl;
		std::cerr << "[xyz] - long option suffixed with x, y or z operates only on that axis." << std::endl;
		std::cerr << "No suffix (or short form) assumes all axes." << std::endl;
		std::cerr << "Example: " << args.app() << " --scale 0.5 model.obj" << std::endl;
		std::cerr << "     or: " << args.app() << " --mirrorx model.obj" << std::endl;
		return args.opt('h', "help") ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	bool info = args.opt('i', "info");
	bool normalize_normals = args.opt('n', "normalize-normals");

	// Output stream handling
	std::ofstream fout;
	std::string outfile = args.arg<std::string>('o', "out");
	std::cerr << outfile << std::endl;
	if (!outfile.empty()) {
		fout.open(outfile.c_str());
		if (fout.fail()) {
			std::cerr << "Failed to open file " << outfile << " for output" << std::endl;
			return EXIT_FAILURE;
		}
	}
	std::ostream& out = outfile.empty() ? std::cout : fout;

	vec3 scale(args.arg('s', "scale", 1.0f));
	scale.x *= args.arg(' ', "scalex", 1.0f);
	scale.y *= args.arg(' ', "scaley", 1.0f);
	scale.z *= args.arg(' ', "scalez", 1.0f);

	vec3 translate(args.arg(' ', "translate", 0.0f));
	translate.x += args.arg(' ', "translatex", 0.0f);
	translate.y += args.arg(' ', "translatey", 0.0f);
	translate.z += args.arg(' ', "translatez", 0.0f);

	vec3 center;
	if (args.opt('c', "center"))  center = vec3(1);
	if (args.opt(' ', "centerx")) center.x = 1;
	if (args.opt(' ', "centery")) center.y = 1;
	if (args.opt(' ', "centerz")) center.z = 1;

	ivec3 mirror(1);
	if (args.opt(' ', "mirror"))  mirror = ivec3(-1);
	if (args.opt(' ', "mirrorx")) mirror.x = -1;
	if (args.opt(' ', "mirrory")) mirror.y = -1;
	if (args.opt(' ', "mirrorz")) mirror.z = -1;

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
			out << APPNAME << " " << VERSION << std::endl;
			out << "Filename: " << filename << std::endl;
			out << std::endl;
			out << "Vertices: " << v_count << std::endl;
			out << "TexCoords: " << vt_count << std::endl;
			out << "Normals: " << vn_count << std::endl;
			out << "Faces: " << f_count << std::endl;
			out << std::endl;
			out << "Center: " << toString((lbound + ubound) * 0.5f) << std::endl;
			out << "Size: " << toString(ubound - lbound) << std::endl;
			out << "Lower bounds: " << toString(lbound) << std::endl;
			out << "Upper bounds: " << toString(ubound) << std::endl;
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
			if (!isZero(center)) in -= center;
			if (!isOne(mirror)) in *= mirror;
			if (!isOne(scale)) in *= scale;
			if (!isZero(rotangles)) in = rotation * in;
			if (!isZero(translate)) in += translate;
			out << "v " << in.x << " " << in.y << " " << in.z << std::endl;
		} else if (row.substr(0,3) == "vn ") {  // Normals
			srow >> tempst >> in.x >> in.y >> in.z;
			if (normalize_normals) in = normalize(in);
			out << "vn " << in.x << " " << in.y << " " << in.z << std::endl;
		} else {
			out << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
