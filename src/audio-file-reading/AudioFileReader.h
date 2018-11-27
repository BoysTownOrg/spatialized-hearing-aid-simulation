#pragma once

#include <common-includes/Interface.h>
#include <string>

class AudioFileReader {
public:
	INTERFACE_OPERATIONS(AudioFileReader);
	virtual long long frames() = 0;
	virtual int channels() = 0;
	virtual void readFrames(float *, long long) = 0;
	virtual bool failed() const = 0;
	virtual std::string errorMessage() const = 0;
};

#include <memory>

class AudioFileReaderFactory {
public:
	INTERFACE_OPERATIONS(AudioFileReaderFactory);
	virtual std::shared_ptr<AudioFileReader> make(std::string filePath) = 0;
};

