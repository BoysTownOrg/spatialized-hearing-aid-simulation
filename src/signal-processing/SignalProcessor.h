#pragma once

#include <common-includes/Interface.h>
#include <gsl/gsl>

class SignalProcessor {
public:
	INTERFACE_OPERATIONS(SignalProcessor);
	virtual void process(gsl::span<float> signal) = 0;
};
