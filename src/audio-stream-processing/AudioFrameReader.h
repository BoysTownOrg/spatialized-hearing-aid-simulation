#pragma once

#include <common-includes/Interface.h>

class AudioFrameReader {
public:
	INTERFACE_OPERATIONS(AudioFrameReader);
	virtual void read(float **audio, int frames) = 0;
	virtual bool complete() const = 0;
	virtual int sampleRate() const = 0;
	virtual int channels() const = 0;
};

