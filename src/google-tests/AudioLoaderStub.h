#pragma once

#include "LogString.h"
#include <playing-audio/AudioLoader.h>

class AudioLoaderStub : public AudioLoader {
	gsl::span<gsl::span<float>> audioBuffer_{};
	LogString log_{};
	int sampleRate_{};
	int channels_{};
	bool complete_{};
public:
	void setComplete() {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}

	const gsl::span<gsl::span<float>> audioBuffer() const {
		return audioBuffer_;
	}

	void load(gsl::span<gsl::span<float>> audio) override {
		audioBuffer_ = std::move(audio);
	}

	void setSampleRate(int r) {
		sampleRate_ = r;
	}

	void setChannels(int c) {
		channels_ = c;
	}

	void reset() override {
		log_ += std::string{ "reset " };
	}

	const LogString &log() const {
		return log_;
	}
};

class InitializationFailingAudioLoader : public AudioLoader {
	std::string errorMessage{};
public:
	explicit InitializationFailingAudioLoader(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	bool complete() override { return {}; }
	void load(gsl::span<gsl::span<float>>) override {}
};