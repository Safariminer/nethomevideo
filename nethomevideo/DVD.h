#pragma once // non standard header guard

#ifndef NETHOMEVIDEO_DVD_H // standard header guard
#define NETHOMEVIDEO_DVD_H

// Standard Template Library
#include <iostream> // for std::string


enum IFOType : char {
	IFO_INVALID,
	IFO_VMG, // VIDEO_TS.IFO
	IFO_VTS // VTS_xx_0.IFO
};

class IFO {
	IFOType type;
	char* ifoContents = nullptr;
public:
	[[deprecated("Use IFO(std::string path) instead")]]
	IFO() { throw std::runtime_error("default IFO ctor"); }
	IFO(std::string path);
	
	IFOType getType();
	char* getIfoContents();

	~IFO();
};


// info about the AUDIO_TS folder
struct AudioTSInfo {
	bool exists = false;
};

// info about the VIDEO_TS folder
struct VideoTSInfo {
	bool exists = false;
	bool hasVMG = false; // checks for VIDEO_TS/VIDEO_TS.IFO
	int titleSetCount = 0;
	IFO *vmg;
};

// info about the DVD's directory's layout
struct DVDDirectoryLayout {
	AudioTSInfo audioTs;
	VideoTSInfo videoTs;
};

// get a corresponding DVDDirectoryLayout for a DVD's root directory
DVDDirectoryLayout* getLayoutInfo(std::string dvdPath);

#endif//NETHOMEVIDEO_DVD_H