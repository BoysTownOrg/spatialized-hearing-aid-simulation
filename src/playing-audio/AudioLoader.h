#pragma once

#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <common-includes/Interface.h>
#include <gsl/gsl>

class AudioLoader {
public:
	INTERFACE_OPERATIONS(AudioLoader);
	virtual void reset() = 0;
	virtual bool complete() = 0;
	using channel_type = gsl::span<float>;
	virtual void load(gsl::span<channel_type> audio) = 0;
	virtual void setReader(std::shared_ptr<AudioFrameReader>) = 0;
	virtual void setProcessor(std::shared_ptr<AudioFrameProcessor>) = 0;
};