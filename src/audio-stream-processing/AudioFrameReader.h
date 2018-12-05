#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class AudioFrameReader {
public:
	INTERFACE_OPERATIONS(AudioFrameReader);
	virtual void read(float **audio, int frames) = 0;
	virtual bool complete() const = 0;
	virtual int sampleRate() const = 0;
	virtual int channels() const = 0;
};

class AudioFrameReaderFactory {
public:
	RUNTIME_ERROR(FileError);
	INTERFACE_OPERATIONS(AudioFrameReaderFactory);
	virtual std::shared_ptr<AudioFrameReader> make(std::string filePath) = 0;
};

