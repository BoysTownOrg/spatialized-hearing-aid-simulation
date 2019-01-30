#pragma once

#include "LogString.h"
#include <playing-audio/AudioLoader.h>

class AudioLoaderStub : public AudioLoader {
	gsl::span<gsl::span<float>> audioBuffer_{};
	LogString log_{};
	std::shared_ptr<AudioFrameReader> audioFrameReader_{};
	std::shared_ptr<AudioFrameProcessor> audioFrameProcessor_{};
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

	auto audioFrameReader() const {
		return audioFrameReader_;
	}

	auto audioFrameProcessor() const {
		return audioFrameProcessor_;
	}

	void setReader(std::shared_ptr<AudioFrameReader> r) override {
		audioFrameReader_ = std::move(r);
	}

	void setProcessor(std::shared_ptr<AudioFrameProcessor> p) override {
		audioFrameProcessor_ = std::move(p);
	}
};
