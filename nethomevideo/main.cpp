// Standard Template Library
#include <iostream> // std::getline() and strings
#include <print> // printing and formatting to the terminal
#include <filesystem> // to check if is directory and if exists
#include <map> // for std::map
#include <typeinfo> // for typeid(...)

// Includes
#include "DVD.h"


 // parse command line arguments following this format
// --key val
std::map<std::string, std::string> parseArgs(int argc, char** argv) {
	
	std::map<std::string, std::string> args;

	std::string key = "", val = "";
	bool inVal = false;

	for (int i = 0; i < argc; i++) {
		std::string arg = std::string(argv[i]);
		if (!inVal) {
			if (arg.starts_with("--")) {
				key = arg.erase(0, 2); // get argument title
				inVal = true;
			}
		}
		else { // get argument value
			if (arg.starts_with("--")) {
				throw std::out_of_range(
					std::format("Need proper value for argument {}", key)
				);
			}
			val = arg;
			args[key] = val;
			inVal = false;
			key = ""; val = "";
		}
	}
	if (key != "") {
		throw std::out_of_range(
			std::format("Need proper value for argument {}", key)
		);
	}
	return args;

}

int main(int argc, char** argv) {
	
	try {
		// parse arguments
		std::map<std::string, std::string> arguments = parseArgs(argc, argv);

		std::string dvdPath = "";

		// --dvd-path argument
		if (arguments.find("dvd-path") != arguments.end()) {

			dvdPath = arguments["dvd-path"];
			std::println(
				"Using path '{}' from command line arguments", 
				dvdPath
			);

		}
		else {
			// message goes over the 80 char limit but needs to be greppable
			std::print(
				"Path not found through arguments(--dvd-path). Enter path to DVD directory > "
			);

			std::getline(std::cin, dvdPath);
		}

		DVDDirectoryLayout* layout = getLayoutInfo(dvdPath);
		std::println("DVD at {} has {} title sets.", 
			dvdPath, layout->videoTs.titleSetCount);
	}
	catch (std::exception& e) {
		std::println("[FATAL] {} : {}", typeid(e).name(), e.what());
		return 1;
	}

}