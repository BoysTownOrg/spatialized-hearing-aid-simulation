#pragma once

#include "LogString.h"
#include <playing-audio/AudioProcessor.h>

class AudioProcessorStub : public AudioProcessor {
	Initialization parameters_{};
	Preparation preparation_{};
	std::vector<int> preferredProcessingSizes_{};
	gsl::span<gsl::span<float>> _audioBuffer{};
	LogString log_{};
	int _sampleRate{};
	int channels_{};
	int chunkSize_{};
	bool complete_{};
public:
	void setPreferredProcessingSizes(std::vector<int> v) {
		preferredProcessingSizes_ = std::move(v);
	}

	std::vector<int> preferredProcessingSizes() override {
		return preferredProcessingSizes_;
	}

	const Initialization &initialization() const {
		return parameters_;
	}

	const Preparation &preparation() const {
		return preparation_;
	}

	void initialize(Initialization p) override {
		parameters_ = std::move(p);
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
		_audioBuffer = std::move(audio);
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

	void setChunkSize(int s) {
		chunkSize_ = s;
	}

	int chunkSize() override {
		return chunkSize_;
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
	int chunkSize() override { return {}; }
	void prepare(Preparation) override {}
	std::vector<int> preferredProcessingSizes() override {
		return {};
	}
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
	int chunkSize() override { return {}; }
	std::vector<int> preferredProcessingSizes() override {
		return {};
	}
};