#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <string>
#include <vector>

struct GlobalTestParameters;

class AudioLoader {
public:
	INTERFACE_OPERATIONS(AudioLoader);
	struct Initialization {
		GlobalTestParameters *global;
	};
	virtual void initialize(Initialization) = 0;
	RUNTIME_ERROR(InitializationFailure);
	
	struct Preparation {
		std::string audioFilePath;
		double level_dB_Spl;
	};
	virtual void prepare(Preparation) = 0;
	RUNTIME_ERROR(PreparationFailure);

	virtual bool complete() = 0;
	virtual void load(gsl::span<gsl::span<float>> audio) = 0;
	virtual int sampleRate() = 0;
	virtual int channels() = 0;
	virtual int bufferSize() = 0;
};