#pragma once // non standard header guard

#ifndef NETHOMEVIDEO_DVD_H // standard header guard
#define NETHOMEVIDEO_DVD_H

// Standard Template Library
#include <iostream> // for std::string


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
DVDDirectoryLayout* getLayoutInfo(std::string dvdPath);


#endif//NETHOMEVIDEO_DVD_H