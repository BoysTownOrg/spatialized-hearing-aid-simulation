#pragma once

#include "LogString.h"
#include <playing-audio/AudioProcessor.h>

class AudioProcessorStub : public AudioProcessor {
	Initialization _parameters{};
	Preparation preparation_{};
	gsl::span<gsl::span<float>> _audioBuffer{};
	LogString log_{};
	int _sampleRate{};
	int channels_{};
	bool complete_{};
public:
	const Initialization &parameters() const {
		return _parameters;
	}

	const Preparation &preparation() const {
		return preparation_;
	}

	void initialize(Initialization p) override {
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
		log_ += std::string{ "channels " };
		return channels_;
	}

	int sampleRate() override {
		log_ += std::string{ "sampleRate " };
		return _sampleRate;
	}

	void prepare(Preparation p) override {
		preparation_ = std::move(p);
		log_ += std::string{ "prepare " };
	}

	const LogString &log() const {
		return log_;
	}
};

class InitializationFailingAudioProcessor : public AudioProcessor {
	std::string errorMessage{};
public:
	explicit InitializationFailingAudioProcessor(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	void initialize(Initialization) override {
		throw InitializationFailure{ errorMessage };
	}

	bool complete() override { return {}; }
	void process(gsl::span<gsl::span<float>>) override {}
	int channels() override { return {}; }
	int sampleRate() override { return {}; }
	void prepare(Preparation) override {}
};

class PreparationFailureAudioProcessor : public AudioProcessor {
	std::string errorMessage{};
public:
	explicit PreparationFailureAudioProcessor(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	void prepare(Preparation) override {
		throw PreparationFailure{ errorMessage };
	}
	
	bool complete() override { return {}; }
	void initialize(Initialization) override {}
	void process(gsl::span<gsl::span<float>>) override {}
	int channels() override { return {}; }
	int sampleRate() override { return {}; }
};