#pragma once

#include <common-includes/Interface.h>

class SignalProcessor {
public:
	INTERFACE_OPERATIONS(SignalProcessor);
	virtual void process(float *, int) = 0;
};
