#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <string>

class AudioFrameReader {
public:
	INTERFACE_OPERATIONS(AudioFrameReader);
	virtual void read(gsl::span<float *> audio, int frames) = 0;
	virtual bool complete() const = 0;
	virtual int sampleRate() const = 0;
	virtual int channels() const = 0;
	virtual long long frames() const = 0;
	virtual void reset() = 0;
};

class AudioFrameReaderFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameReaderFactory);
	RUNTIME_ERROR(CreateError);
	virtual std::shared_ptr<AudioFrameReader> make(std::string filePath) = 0;
};

