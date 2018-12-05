#pragma once

#include <playing-audio/AudioFrameProcessor.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	int _frameCount{};
	float **_channels{};
public:
	int frameCount() const {
		return _frameCount;
	}

	const float *const *channels() const {
		return _channels;
	}

	void process(float ** channels, int frameCount) override {
		_channels = channels;
		_frameCount = frameCount;
	}
};

class AudioFrameProcessorStubFactory : public AudioFrameProcessorFactory {
	Parameters _parameters{};
	std::shared_ptr<AudioFrameProcessor> processor;
public:
	explicit AudioFrameProcessorStubFactory(
		std::shared_ptr<AudioFrameProcessor> processor =
		std::make_shared<AudioFrameProcessorStub>()
	) :
		processor{ std::move(processor) } {}

	const Parameters &parameters() const {
		return _parameters;
	}

	std::shared_ptr<AudioFrameProcessor> make(Parameters p) override {
		_parameters = p;
		return processor;
	}
};