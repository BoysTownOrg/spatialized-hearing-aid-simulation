#pragma once

#include <playing-audio/AudioFrameProcessor.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	gsl::span<gsl::span<float>> _audioBuffer{};
	int groupDelay_{};
	bool complete_{};
public:
	const gsl::span<gsl::span<float>> audioBuffer() const {
		return _audioBuffer;
	}

	void process(gsl::span<gsl::span<float>> audio) override {
		_audioBuffer = audio;
	}

	void setGroupDelay(int n) {
		groupDelay_ = n;
	}

	int groupDelay() override {
		return groupDelay_;
	}

	void setComplete() {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}
};

class AudioFrameProcessorStubFactory : public AudioFrameProcessorFactory {
	Parameters _parameters{};
	gsl::span<gsl::span<float>> _audioBuffer{};
	std::shared_ptr<AudioFrameProcessor> processor;
	bool complete_{};
public:
	explicit AudioFrameProcessorStubFactory(
		std::shared_ptr<AudioFrameProcessor> processor =
			std::make_shared<AudioFrameProcessorStub>()
	) :
		processor{ std::move(processor) } {}

	void setProcessor(std::shared_ptr<AudioFrameProcessor> p) {
		this->processor = std::move(p);
	}

	const Parameters &parameters() const {
		return _parameters;
	}

	std::shared_ptr<AudioFrameProcessor> make(Parameters p) override {
		_parameters = p;
		return processor;
	}

	void setComplete() {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}

	const gsl::span<gsl::span<float>> audioBuffer() const {
		return _audioBuffer;
	}

	void process(gsl::span<gsl::span<float>> audio) override {
		_audioBuffer = audio;
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

	bool complete() override {
		return {};
	}

	void process(gsl::span<gsl::span<float>>) override {}
};