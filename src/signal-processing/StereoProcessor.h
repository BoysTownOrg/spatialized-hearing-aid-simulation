#pragma once

#include <common-includes/Interface.h>

class StereoProcessor {
public:
	INTERFACE_OPERATIONS(StereoProcessor);
	virtual void process(float *xLeft, float *xRight, int samples) = 0;
};
