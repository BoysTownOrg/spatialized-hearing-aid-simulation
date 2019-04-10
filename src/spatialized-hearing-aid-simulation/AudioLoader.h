#pragma once

#include "AudioFrameProcessor.h"
#include <spatialized-hearing-aid-simulation/AudioFrameReader.h>
#include <common-includes/Interface.h>
#include <gsl/gsl>

class AudioLoader {
public:
    INTERFACE_OPERATIONS(AudioLoader)
	virtual bool complete() = 0;
	using channel_type = gsl::span<float>;
	virtual void load(gsl::span<channel_type> audio) = 0;
};
