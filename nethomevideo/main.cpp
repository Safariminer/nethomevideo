#include <iostream> // std::getline() and strings
#include <print> // printing and formatting to the terminal
#include <filesystem> // to check if is directory and if exists
#include <map> // for std::map
#include <typeinfo> // for typeid(...)

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

// info about the AUDIO_TS folder
struct AudioTSInfo {
	bool exists = false;
};

// info about the VIDEO_TS folder
struct VideoTSInfo {
	bool exists = false;
};

// info about the DVD's directory's layout
struct DVDDirectoryLayout {
	AudioTSInfo audioTs;
	VideoTSInfo videoTs;
};

// get a corresponding DVDDirectoryLayout for a DVD's root directory
DVDDirectoryLayout* getLayoutInfo(std::string dvdPath) {
	namespace fs = std::filesystem;
	
	// preliminary checks
	if (!fs::exists(dvdPath)) {
		throw std::out_of_range(
			std::format("Path {} does not exist.", dvdPath)
		);
	}

	if (!fs::is_directory(dvdPath)) {
		throw std::logic_error(
			std::format("Path {} is not a directory.", dvdPath)
		);
	}

	const std::string videoTsPath = dvdPath + "\\VIDEO_TS";
	const std::string audioTsPath = dvdPath + "\\AUDIO_TS";


	DVDDirectoryLayout* layout = new DVDDirectoryLayout();
	int properDVDDirectories = 0;

	
	 // check for VIDEO_TS directory in <dvdPath>/
	// [CRITICAL]
	if (fs::exists(videoTsPath)) {
		if (fs::is_directory(videoTsPath)) {
			layout->videoTs.exists = true;
			std::println("'VIDEO_TS' directory found within {}.", dvdPath);
		}
		else {
			throw std::logic_error(
				std::format(
					"Path {} contains 'VIDEO_TS', but it isn't a directory.",
					dvdPath
				)
			);
		}
	}
	else {
		throw std::out_of_range(
			std::format(
				"Path {} lacks 'VIDEO_TS' directory. It cannot be a DVD.",
				dvdPath
			)
		);
	}

	 // check for AUDIO_TS directory in <dvdPath>/
	// [OPTIONAL]
	if (fs::exists(audioTsPath)) {
		if (fs::is_directory(audioTsPath)) {
			layout->audioTs.exists = true;
			std::println("'AUDIO_TS' directory found within {}.", dvdPath);
		}
		else {
			std::println(
				"Path {} contains 'AUDIO_TS', but it isn't a directory.",
				dvdPath
			);
		}
	}
	else {
		std::println(
			"Path {} lacks 'AUDIO_TS' directory.",
			dvdPath
		);
	}

	return layout;
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
	}
	catch (std::exception& e) {
		std::println("[FATAL] {} : {}", typeid(e).name(), e.what());
		return 1;
	}

}