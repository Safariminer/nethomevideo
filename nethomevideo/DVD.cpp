// Precising header
#include "DVD.h"

// Standard Template Library
#include <iostream>
#include <filesystem>
#include <print>
#include <fstream>

// Fill out a VideoTSInfo struct from a "[...]/VIDEO_TS" path
void FillOutVideoTS(VideoTSInfo* info, std::string path) {
	namespace fs = std::filesystem;

	const std::string vmgPath = path + "\\VIDEO_TS.IFO";

	if (fs::exists(vmgPath)) {
		if (fs::is_regular_file(vmgPath)) {
			std::println("VMG(VIDEO_TS.IFO) successfully found");
			info->hasVMG = true;

			// read vmg
			const size_t vmgSize = fs::file_size(vmgPath);
			char* ifoContents = (char*)malloc(vmgSize);
			std::ifstream in(vmgPath);
			in.read(ifoContents, vmgSize);
			in.close();

			// assume little endian
			unsigned short titleSetFromVMG;
			memcpy(&titleSetFromVMG, ifoContents + 0x003e, 2);
			titleSetFromVMG = _byteswap_ushort(titleSetFromVMG);
			
			info->titleSetCount = (int)titleSetFromVMG;

			// discard vmg
			free(ifoContents);

		}
		else {
			throw std::logic_error(
				std::format(
					"VMG(VIDEO_TS.IFO) found in {}, but not a regular file. Could be a directory or a device.",
					path
				)
			);
		}
	}
	else {
		throw std::out_of_range(
			std::format(
				"No VMG(VIDEO_TS.IFO) found in {}. Cannot be a valid DVD.",
				path
			)
		);
	}

}

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
			std::println("'VIDEO_TS' directory found within {}.", dvdPath);
			layout->videoTs.exists = true;
			FillOutVideoTS(&(layout->videoTs), videoTsPath);
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