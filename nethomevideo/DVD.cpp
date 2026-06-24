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
			info->vmg = new IFO(vmgPath);

			// assume little endian
			unsigned short titleSetFromVMG;
			memcpy(&titleSetFromVMG, info->vmg->getIfoContents() + 0x003e, 2);
			titleSetFromVMG = _byteswap_ushort(titleSetFromVMG);
			
			info->titleSetCount = (int)titleSetFromVMG;

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

IFO::IFO(std::string path)
{
	namespace fs = std::filesystem;

	if (!fs::exists(path)) {
		throw std::out_of_range(
			std::format("{} does not exist.", path)
		);
	}

	if (!fs::is_regular_file(path)) {
		throw std::out_of_range(
			std::format(
				"{} is not a regular file. Could be a directory or a device.", 
				path
			)
		);
	}

	// get file size and allocate
	const size_t fileSize = fs::file_size(path);
	if (fileSize < 0x03D8 + 40) {
		throw std::out_of_range(
			std::format(
				"File {} too small to be a valid IFO file.",
				path
			)
		);
	}

	ifoContents = (char*)malloc(fileSize);
	
	if (ifoContents == NULL) {
		throw std::runtime_error("malloc failed");
	}

	// grab data from file and close as quickly as possible
	std::ifstream in(path);
	in.read(ifoContents, fileSize); in.close();

	// validate
	constexpr char ifoMagic[9] = 
		{ 'D', 'V', 'D', 'V', 'I', 'D', 'E', 'O', '-' };

	constexpr char vmgMagic[3] = { 'V', 'M', 'G' };
	constexpr char vtsMagic[3] = { 'V', 'T', 'S' };

	if (memcmp(ifoMagic, ifoContents, 9) != 0) {
		throw std::logic_error(
			std::format(
				"File {} isn't a valid DVD-Video IFO file...",
				path
			)
		);
	}


	type = IFO_INVALID;

	if (memcmp(vmgMagic, ifoContents + 9, 3) == 0) type = IFO_VMG;
	if (memcmp(vtsMagic, ifoContents + 9, 3) == 0) type = IFO_VTS;

	if (type == IFO_INVALID) {
		throw std::logic_error(
			std::format(
				"File {} is unsupported (neither VMG or VTS IFO)",
				path
			)
		);
	}

}

IFOType IFO::getType()
{
	return type;
}

char* IFO::getIfoContents()
{
	return ifoContents;
}

IFO::~IFO()
{
	if (ifoContents != nullptr && ifoContents != NULL) {
		free(ifoContents);
	}
}
