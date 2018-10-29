#pragma once

#include <common-includes/Interface.h>

class AudioProcessor {
public:
	INTERFACE_OPERATIONS(AudioProcessor);
	virtual void process(float **channels, int frameCount) = 0;
};