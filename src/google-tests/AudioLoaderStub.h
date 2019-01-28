#pragma once

#include "LogString.h"
#include <playing-audio/AudioLoader.h>

class AudioLoaderStub : public AudioLoader {
	Preparation preparation_{};
	gsl::span<gsl::span<float>> audioBuffer_{};
	LogString log_{};
	int sampleRate_{};
	int channels_{};
	int bufferSize_{};
	bool complete_{};
public:
	const Preparation &preparation() const {
		return preparation_;
	}

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

	int channels() override {
		log_ += std::string{ "channels " };
		return channels_;
	}

	int sampleRate() override {
		log_ += std::string{ "sampleRate " };
		return sampleRate_;
	}

	void prepare(Preparation p) override {
		preparation_ = std::move(p);
		log_ += std::string{ "prepare " };
	}

	const LogString &log() const {
		return log_;
	}

	void setBufferSize(int s) {
		bufferSize_ = s;
	}

	int bufferSize() override {
		return bufferSize_;
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
	int channels() override { return {}; }
	int sampleRate() override { return {}; }
	int bufferSize() override { return {}; }
	void prepare(Preparation) override {}
};

class PreparationFailureAudioLoader : public AudioLoader {
	std::string errorMessage{};
public:
	explicit PreparationFailureAudioLoader(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	void prepare(Preparation) override {
		throw PreparationFailure{ errorMessage };
	}
	
	bool complete() override { return {}; }
	void load(gsl::span<gsl::span<float>>) override {}
	int channels() override { return {}; }
	int sampleRate() override { return {}; }
	int bufferSize() override { return {}; }
};