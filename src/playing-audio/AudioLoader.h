#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <string>
#include <vector>

class AudioLoader {
public:
	struct Preparation {
		std::string audioFilePath;
	};
	virtual void prepare(Preparation) = 0;
	RUNTIME_ERROR(PreparationFailure);

	virtual bool complete() = 0;
	using channel_type = gsl::span<float>;
	virtual void load(gsl::span<channel_type> audio) = 0;
	virtual int sampleRate() = 0;
	virtual int channels() = 0;
	virtual int bufferSize() = 0;
};