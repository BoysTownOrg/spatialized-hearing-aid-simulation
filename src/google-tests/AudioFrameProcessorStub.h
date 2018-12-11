#pragma once

#include <playing-audio/AudioFrameProcessor.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	int _frameCount{};
	float **_audioBuffer{};
public:
	int frames() const {
		return _frameCount;
	}

	const float *const *audioBuffer() const {
		return _audioBuffer;
	}

	void process(gsl::span<float *> channels, int frames) override {
		_audioBuffer = channels.data();
		_frameCount = frames;
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

class ErrorAudioFrameProcessorFactory : public AudioFrameProcessorFactory {
	std::string errorMessage{};
public:
	explicit ErrorAudioFrameProcessorFactory(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<AudioFrameProcessor> make(Parameters) override {
		throw CreateError{ errorMessage };
	}
};