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
	using channel_type = gsl::span<float>;
	virtual void process(gsl::span<channel_type> audio) = 0;
	virtual channel_type::index_type groupDelay() = 0;
};

struct GlobalTestParameters;

class AudioFrameProcessorFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessorFactory);
	RUNTIME_ERROR(CreateError);
	struct Parameters {
		std::vector<double> channelScalars;
		double max_dB_Spl;
		int sampleRate;
		int channels;
		GlobalTestParameters *global;
	};
	virtual std::shared_ptr<AudioFrameProcessor> make(Parameters) = 0;
	virtual std::vector<int> preferredProcessingSizes() = 0;
	virtual int preferredBufferSize() = 0;
};
