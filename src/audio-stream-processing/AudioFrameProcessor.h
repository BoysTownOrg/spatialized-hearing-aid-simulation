#pragma once

#include <common-includes/Interface.h>
#include <string>

class AudioFrameProcessor {
public:
	struct Parameters {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
		int sampleRate;
	};
	INTERFACE_OPERATIONS(AudioFrameProcessor);
	virtual void process(float **channels, int frameCount) = 0;
};

#include <memory>

class AudioFrameProcessorFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessorFactory);
	virtual std::shared_ptr<AudioFrameProcessor> make(AudioFrameProcessor::Parameters) = 0;
};
