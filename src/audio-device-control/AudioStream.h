#pragma once

#include <common-includes/Interface.h>

class AudioStream {
public:
	INTERFACE_OPERATIONS(AudioStream);
	virtual void fillBuffer(float **channels, int frameCount) = 0;
};

