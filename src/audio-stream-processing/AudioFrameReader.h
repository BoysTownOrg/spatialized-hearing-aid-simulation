#pragma once

#include <common-includes/Interface.h>

class AudioFrameReader {
public:
	INTERFACE_OPERATIONS(AudioFrameReader);
	virtual void read(float **frames, int frameCount) = 0;
};

