#pragma once

#include <common-includes/Interface.h>

class AudioFrameReader {
public:
	INTERFACE_OPERATIONS(AudioFrameReader);
	virtual void read(float **channels, int frameCount) = 0;
};

#include <memory>
#include <string>

class AudioFrameReaderFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameReaderFactory);
	virtual std::shared_ptr<AudioFrameReader> make(std::string filePath) = 0;
};

