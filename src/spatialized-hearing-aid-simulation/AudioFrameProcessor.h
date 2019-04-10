#pragma once

#include <common-includes/Interface.h>
#include <gsl/gsl>

class AudioFrameProcessor {
public:
    INTERFACE_OPERATIONS(AudioFrameProcessor)
	using channel_type = gsl::span<float>;
	virtual void process(gsl::span<channel_type> audio) = 0;
	virtual channel_type::index_type groupDelay() = 0;
};
