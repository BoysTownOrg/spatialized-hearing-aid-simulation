#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <string>
#include <vector>

class AudioLoader {
public:
	INTERFACE_OPERATIONS(AudioLoader);
	struct Initialization {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double max_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
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
	virtual int chunkSize() = 0;
	virtual std::vector<int> preferredProcessingSizes() = 0;
};