#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <map>

#include "../glm/vec2.hpp"
#include "../glm/vec3.hpp"
#include "../glm/mat3x3.hpp"
#include "../glm/mat4x4.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/component_wise.hpp"
#include "args.hpp"

#define APPNAME "obj-magic"
#define VERSION "v0.5"

#define EPSILON 0.00001f
#define W 12

using namespace glm;

std::string toString(vec3 vec) {
	std::ostringstream oss;
	oss << std::right << std::setw(W) << vec.x << std::setw(W) << vec.y << std::setw(W) << vec.z;
	return oss.str();
}

template<typename T> inline bool isZero(T v) { v = abs(v); return v.x < EPSILON && v.y < EPSILON && v.z < EPSILON; }
template<typename T> inline bool isOne(T v) { return isZero(v - T(1)); }
template<typename T> inline bool isEqual(T a, T b) { return isZero(a - b); }

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
		std::cerr << " -O   --overwrite               edit input file directly, overwriting it" << std::endl;
		std::cerr << " -i   --info                    print info about the object and exit" << std::endl;
		std::cerr << " -n   --normalize-normals       renormalize all normals" << std::endl;
		std::cerr << " -n   --invert-normals          invert all normals" << std::endl;
		std::cerr << " -c   --center[xyz]             center object" << std::endl;
		std::cerr << " -s   --scale[xyz] SCALE        scale object SCALE amount" << std::endl;
		std::cerr << "      --scaleuv[xy] SCALE       multiply texture coords by SCALE amount" << std::endl;
		std::cerr << "      --invertuv[xy]            make texture coord 1-original" << std::endl;
		std::cerr << "      --mirror[xyz]             mirror object" << std::endl;
		std::cerr << "      --translate[xyz] AMOUNT   translate AMOUNT amount" << std::endl;
		std::cerr << "      --rotate[xyz] AMOUNT      rotate along axis AMOUNT degrees" << std::endl;
		std::cerr << "      --fit[xyz] AMOUNT         uniformly scale to fit AMOUNT in dimension" << std::endl;
		std::cerr << std::endl;
		std::cerr << "[xyz] - long option suffixed with x, y or z operates only on that axis." << std::endl;
		std::cerr << "No suffix (or short form) assumes all axes." << std::endl;
		std::cerr << "Example: " << args.app() << " --scale 0.5 model.obj" << std::endl;
		std::cerr << "     or: " << args.app() << " --mirrorx model.obj" << std::endl;
		return args.opt('h', "help") ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	bool info = args.opt('i', "info");
	bool normalize_normals = args.opt('n', "normalize-normals");
	vec3 normal_scale = args.opt(' ', "invert-normals") ? vec3(-1.0f) : vec3(1.0);

	// Output stream handling
	std::string infile = argv[argc-1];
	std::string outfile = args.arg<std::string>('o', "out");
	std::ofstream fout;
	std::stringstream sout;
	bool inPlaceOutput = false;
	if (outfile == infile || args.opt('O', "overwrite")) { // In-place
		inPlaceOutput = true;
	} else if (!outfile.empty()) {
		fout.open(outfile.c_str());
		if (fout.fail()) {
			std::cerr << "Failed to open file " << outfile << " for output" << std::endl;
			return EXIT_FAILURE;
		}
	}
	std::ostream& out = inPlaceOutput ? sout : (outfile.empty() ? std::cout : fout);

	vec3 scale(args.arg('s', "scale", 1.0f));
	scale.x *= args.arg(' ', "scalex", 1.0f);
	scale.y *= args.arg(' ', "scaley", 1.0f);
	scale.z *= args.arg(' ', "scalez", 1.0f);

	vec2 scaleUv(args.arg(' ', "scaleuv", 1.0f));
	scaleUv.x *= args.arg(' ', "scaleuvx", 1.0f);
	scaleUv.y *= args.arg(' ', "scaleuvy", 1.0f);

	bool flipUvX = args.opt(' ', "invertuv") || args.opt(' ', "invertuvx");
	bool flipUvY = args.opt(' ', "invertuv") || args.opt(' ', "invertuvy");

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

	vec3 fit;
	fit.x = args.arg(' ', "fitx", 0.0f);
	fit.y = args.arg(' ', "fity", 0.0f);
	fit.z = args.arg(' ', "fitz", 0.0f);
	if (args.arg(' ', "fit", 0.0f))
		fit = vec3(args.arg(' ', "fit", 0.0f));

	vec3 rotangles(args.arg(' ', "rotate", 0.0f));
	rotangles.x += args.arg(' ', "rotatex", 0.0f);
	rotangles.y += args.arg(' ', "rotatey", 0.0f);
	rotangles.z += args.arg(' ', "rotatez", 0.0f);
	mat4 temprot(1.0f);
	if (rotangles.x != 0.0f) temprot = rotate(temprot, rotangles.x, vec3(1,0,0));
	if (rotangles.y != 0.0f) temprot = rotate(temprot, rotangles.y, vec3(0,1,0));
	if (rotangles.z != 0.0f) temprot = rotate(temprot, rotangles.z, vec3(0,0,1));
	mat3 rotation(temprot);

	std::ifstream file(infile.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file " << infile << std::endl;
		return EXIT_FAILURE;
	}

	std::string row;
	// Analyzing pass
	bool analyze = info || (center.length() > 0.0f) || (fit.length() > 0.0f);
	if (analyze) {
		vec3 lbound(std::numeric_limits<float>::max());
		vec3 ubound(-std::numeric_limits<float>::max());
		std::map<std::string, unsigned> materials;
		unsigned long long v_count = 0, vt_count = 0, vn_count = 0, f_count = 0, p_count = 0, l_count = 0, o_count = 0;
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
			else if (row.substr(0,2) == "p ") ++p_count;
			else if (row.substr(0,2) == "l ") ++l_count;
			else if (row.substr(0,2) == "f ") ++f_count;
			else if (row.substr(0,2) == "o ") ++o_count;
			else if (row.substr(0,7) == "usemtl ") materials[row.substr(7)]++;
		}
		center *= (lbound + ubound) * 0.5f;
		// Output info?
		if (info) {
			out << APPNAME << " " << VERSION << std::endl;
			out << "Filename:      " << infile << std::endl;
			out << "Vertices:      " << v_count << std::endl;
			out << "TexCoords:     " << vt_count << std::endl;
			out << "Normals:       " << vn_count << std::endl;
			out << "Faces:         " << f_count << std::endl;
			out << "Points:        " << p_count << std::endl;
			out << "Lines:         " << l_count << std::endl;
			out << "Named objects: " << o_count << std::endl;
			out << "Materials:     " << materials.size() << std::endl;
			out << "              " << std::right << std::setw(W) << "x" << std::setw(W) << "y" << std::setw(W) << "z" << std::endl;
			out << "Center:       " << toString((lbound + ubound) * 0.5f) << std::endl;
			out << "Size:         " << toString(ubound - lbound) << std::endl;
			out << "Lower bounds: " << toString(lbound) << std::endl;
			out << "Upper bounds: " << toString(ubound) << std::endl;
			return EXIT_SUCCESS;
		}
		if (fit.length()) {
			vec3 size = ubound - lbound;
			float fitScale = 1.f;
			if (args.arg(' ', "fit", 0.f)) fitScale = args.arg(' ', "fit", 0.f) / compMax(size);
			else if (fit.x) fitScale = fit.x / size.x;
			else if (fit.y) fitScale = fit.y / size.y;
			else if (fit.z) fitScale = fit.z / size.z;
			scale *= fitScale;
		}
	}
	
	auto outputUnmodifiedRow = [](std::ostream& out, const std::string& row) {
		// getline stops at \n, so there might be \r hiding in there if we are reading CRLF files
		int last = row.size() - 1;
		if (last >= 0 && row[last] == '\r')
			out << row.substr(0, last) << std::endl;
		else out << row << std::endl;
	};

	// Output pass
	file.clear();
	file.seekg(0, std::ios::beg);
	while (getline(file, row)) {
		std::istringstream srow(row);
		vec3 in;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> in.x >> in.y >> in.z;
			vec3 old = in;
			in -= center;
			in *= mirror;
			in *= scale;
			in = rotation * in;
			in += translate;
			if (old != in)
				out << "v " << in.x << " " << in.y << " " << in.z << std::endl;
			else outputUnmodifiedRow(out, row);
		} else if (row.substr(0,3) == "vt ") {  // Tex coords
			srow >> tempst >> in.x >> in.y;
			vec3 old = in;
			if (flipUvX) in.x = 1.0f - in.x;
			if (flipUvY) in.y = 1.0f - in.y;
			in.x *= scaleUv.x;
			in.y *= scaleUv.y;
			if (old != in)
				out << "vt " << in.x << " " << in.y << std::endl;
			else outputUnmodifiedRow(out, row);
		} else if (row.substr(0,3) == "vn ") {  // Normals
			srow >> tempst >> in.x >> in.y >> in.z;
			vec3 old = in;
			in *= normal_scale;
			if (normalize_normals) in = normalize(in);
			if (old != in)
				out << "vn " << in.x << " " << in.y << " " << in.z << std::endl;
			else outputUnmodifiedRow(out, row);
		} else {
			outputUnmodifiedRow(out, row);
		}
	}

	if (inPlaceOutput) {
		file.close();
		fout.open(infile.c_str());
		if (fout.fail()) {
			std::cerr << "Failed to open file " << infile << " for output" << std::endl;
			return EXIT_FAILURE;
		}
		fout << sout.rdbuf();
	}

	return EXIT_SUCCESS;
}
