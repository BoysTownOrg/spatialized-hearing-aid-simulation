#pragma once

#include <common-includes/Interface.h>

class AudioFrameProcessor {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessor);
	virtual void process(float **channels, int frameCount) = 0;
};

#include <common-includes/RuntimeError.h>
#include <memory>
#include <string>

class AudioFrameProcessorFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessorFactory);
	RUNTIME_ERROR(Failure);
	struct Parameters {
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