#pragma once

#include <common-includes/Interface.h>
#include <gsl/gsl>

class SignalProcessor {
public:
    INTERFACE_OPERATIONS(SignalProcessor)
	using signal_type = gsl::span<float>;
	using index_type = signal_type::index_type;
	virtual void process(signal_type signal) = 0;
	virtual index_type groupDelay() = 0;
};
