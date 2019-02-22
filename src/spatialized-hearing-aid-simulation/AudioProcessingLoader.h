#pragma once

#include "AudioLoader.h"

class AudioProcessingLoader : public AudioLoader {
public:
	virtual void reset() = 0;
	virtual void setReader(std::shared_ptr<AudioFrameReader>) = 0;
	virtual void setProcessor(std::shared_ptr<AudioFrameProcessor>) = 0;
};