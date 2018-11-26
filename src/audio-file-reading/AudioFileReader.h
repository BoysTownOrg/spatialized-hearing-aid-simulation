#pragma once

#include <common-includes/Interface.h>

class AudioFileReader {
public:
	INTERFACE_OPERATIONS(AudioFileReader);
	virtual long long frames() = 0;
	virtual int channels() = 0;
	virtual void readFrames(float *, long long) = 0;
};

#include <memory>
#include <string>

class AudioFileReaderFactory {
public:
	INTERFACE_OPERATIONS(AudioFileReaderFactory);
	virtual std::shared_ptr<AudioFileReader> make(std::string filePath) = 0;
};

