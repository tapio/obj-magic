#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage:    " << argv[0] << " FILE SCALE" << std::endl;
		std::cerr << "Example:  " << argv[0] << " model.obj 0.5" << std::endl;
		return EXIT_FAILURE;
	}
	float scale = atof(argv[2]);
	if (scale <= 0.0f) {
		std::cerr << "Invalid scale" << std::endl;
		return EXIT_FAILURE;
	}
	std::string row;
	std::ifstream file(argv[1], std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}
	while (getline(file, row)) {
		std::istringstream srow(row);
		float x,y,z;
		std::string tempst;
		if (row.substr(0,2) == "v ") {  // Vertices
			srow >> tempst >> x >> y >> z;
			std::cout << "v " << x*scale << " " << y*scale << " " << z*scale << std::endl;
		} else {
			std::cout << row << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
