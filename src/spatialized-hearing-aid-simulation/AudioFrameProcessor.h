#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <memory>
#include <string>
#include <vector>

class AudioFrameProcessor {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessor);
	using channel_type = gsl::span<float>;
	virtual void process(gsl::span<channel_type> audio) = 0;
	virtual channel_type::index_type groupDelay() = 0;
};
