// Precising header
#include "DVD.h"

// Standard Template Library
#include <iostream>
#include <filesystem>
#include <print>

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