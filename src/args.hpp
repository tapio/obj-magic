#pragma once

#include <string>
#include <sstream>
#include <set>
#include <vector>

class Args {
public:
	Args(int argc, char* argv[]): app_name(argv[0])
	{
		for (int i = 1; i < argc; ++i) {
			std::string arg(argv[i]);
			int l = arg.length();
			if (l == 0) continue;
			if (arg[0] == '-' && l >= 2) {
				if (arg[1] == '-') { // Long opt
					arg = arg.substr(2);
					size_t pos = arg.find('=');
					if (pos != std::string::npos) {
						longopts.insert(arg.substr(0, pos));
						allopts.push_back("--" + arg.substr(0, pos));
						allopts.push_back(arg.substr(pos+1));
						continue;
					}
					longopts.insert(arg);
					allopts.push_back("--" + arg);
				} else { // Short opt
					for (int j = 1; j < l; ++j) {
						if (arg[j] == '=') { // Handle argument format: -a=123
							allopts.push_back(arg.substr(j+1));
							break;
						}
						shortopts.insert(arg[j]);
						allopts.push_back("-" + arg.substr(j,1));
					}
				}
			} else if (arg[0] != '-') {
				globalopts.insert(arg);
				allopts.push_back(arg);
			}
		}
	}

	bool opt(char shortopt, std::string longopt) {
		if (shortopt && shortopt != ' ' && shortopts.count(shortopt)) return true;
		if (!longopt.empty() && longopts.count(longopt)) return true;
		return false;
	}

	template<typename T> // std::string specialized below this class
	T arg(char shortopt, std::string longopt, T default_arg = T()) {
		for (std::vector<std::string>::const_iterator it = allopts.begin(); it != allopts.end(); ++it) {
			if (*it == "-" + std::string(1, shortopt) || *it == "--" + longopt) {
				++it;
				if (it == allopts.end()) return default_arg;
				T ret = T();
				std::istringstream iss(*it);
				iss >> ret;
				if (!iss.fail()) return ret;
				else return default_arg;
			}
		}
		return default_arg;
	}

	const std::set<std::string>& orphans() const { return globalopts; }

	std::string app() const { return app_name; }

private:
	std::string app_name;
	std::vector<std::string> allopts;
	std::set<char> shortopts;
	std::set<std::string> longopts;
	std::set<std::string> globalopts;
};


template<>
std::string Args::arg<std::string>(char shortopt, std::string longopt, std::string default_arg) {
	for (std::vector<std::string>::const_iterator it = allopts.begin(); it != allopts.end(); ++it) {
		if (*it == "-" + std::string(1, shortopt) || *it == "--" + longopt) {
			++it;
			if (it == allopts.end() || (*it)[0] == '-') return default_arg;
			return *it;
		}
	}
	return default_arg;
}
