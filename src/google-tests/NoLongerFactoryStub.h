#pragma once

#include <playing-audio/AudioPlayer.h>

class NoLongerFactoryStub : public NoLongerFactory {
	Initialization _parameters{};
	std::string audioFilePath_{};
	gsl::span<gsl::span<float>> _audioBuffer{};
	int _sampleRate{};
	int channels_{};
	bool complete_{};
public:
	const Initialization &parameters() const {
		return _parameters;
	}

	void make(Initialization p) override {
		_parameters = p;
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

	void make(Initialization) override {
		throw CreateError{ errorMessage };
	}

	bool complete() override {
		return {};
	}

	void process(gsl::span<gsl::span<float>>) override {}
	int channels() override { return {}; }
	int sampleRate() override { return {}; }
};