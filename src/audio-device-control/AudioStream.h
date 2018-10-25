#pragma once

#include <common-includes/Interface.h>

class AudioStream {
public:
	INTERFACE_OPERATIONS(AudioStream);
	virtual void fillBuffer(float *left, float *right, int frameCount) = 0;
};

