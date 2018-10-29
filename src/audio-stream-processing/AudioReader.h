#pragma once

#include <common-includes/Interface.h>

class AudioReader {
public:
	INTERFACE_OPERATIONS(AudioReader);
	virtual void read(float **channels, int frameCount) = 0;
};