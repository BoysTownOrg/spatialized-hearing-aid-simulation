#pragma once

#include <common-includes/Interface.h>

class StereoAudioFile {
public:
	INTERFACE_OPERATIONS(StereoAudioFile);
	virtual void read(float *left, float *right, int samples) = 0;
};
