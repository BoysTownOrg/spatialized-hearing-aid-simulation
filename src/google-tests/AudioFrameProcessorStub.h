#pragma once

#include <playing-audio/NoLongerFactory.h>

class NoLongerFactoryStub : public NoLongerFactory {
	Parameters _parameters{};
	std::string audioFilePath_{};
	gsl::span<gsl::span<float>> _audioBuffer{};
	std::shared_ptr<AudioFrameProcessor> processor;
	int _sampleRate{};
	int channels_{};
	bool complete_{};
public:
	explicit NoLongerFactoryStub(
		std::shared_ptr<AudioFrameProcessor> processor = {}
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

	void setSampleRate(int r) {
		_sampleRate = r;
	}

	void setChannels(int c) {
		channels_ = c;
	}

	int channels() override {
		return channels_;
	}

	int sampleRate() override {
		return _sampleRate;
	}

	std::string audioFilePath() {
		return _parameters.audioFilePath;
	}
};

class ErrorNoLongerFactory : public NoLongerFactory {
	std::string errorMessage{};
public:
	explicit ErrorNoLongerFactory(
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
	int channels() override { return {}; }
	int sampleRate() override { return {}; }
};