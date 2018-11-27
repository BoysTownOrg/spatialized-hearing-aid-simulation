#pragma once

#include <common-includes/Interface.h>

class StereoAudioFile {
public:
	INTERFACE_OPERATIONS(StereoAudioFile);
	virtual void read(float *left, float *right, int samples) = 0;
};

#include "AudioFileReader.h"
#include <memory>

class StereoAudioFileFactory {
public:
	INTERFACE_OPERATIONS(StereoAudioFileFactory);
	virtual std::shared_ptr<StereoAudioFile> make(AudioFileReader &) = 0;
};
