#pragma once

#include <common-includes/Interface.h>
#include <string>

class AudioFrameProcessor {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessor);
	virtual void process(float **channels, int frameCount) = 0;
};
