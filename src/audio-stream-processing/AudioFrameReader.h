#pragma once

#include <common-includes/Interface.h>

class AudioFrameReader {
public:
	INTERFACE_OPERATIONS(AudioFrameReader);
	virtual void read(float **channels, int frameCount) = 0;
	virtual bool complete() const = 0;
};

