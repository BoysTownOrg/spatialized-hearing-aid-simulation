#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <memory>
#include <string>
#include <vector>

class AudioFrameProcessor {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessor);
	virtual void process(gsl::span<gsl::span<float>> audio) = 0;
};

class AudioFrameProcessorFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessorFactory);
	RUNTIME_ERROR(CreateError);
	struct Parameters {
		std::vector<double> stimulusRms;
		std::vector<double> channelScalars;
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int chunkSize;
		int windowSize;
		int sampleRate;
		int channels;
	};
	virtual std::shared_ptr<AudioFrameProcessor> make(Parameters) = 0;
};