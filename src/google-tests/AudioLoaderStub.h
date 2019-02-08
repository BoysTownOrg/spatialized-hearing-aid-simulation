#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/AudioLoader.h>

class AudioLoaderStub : public AudioLoader {
	LogString log_{};
	gsl::span<channel_type> audioBuffer_{};
	std::shared_ptr<AudioFrameReader> audioFrameReader_{};
	std::shared_ptr<AudioFrameProcessor> audioFrameProcessor_{};
	int sampleRate_{};
	int channels_{};
	bool complete_{};
public:
	void setComplete() noexcept {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}

	auto audioBuffer() const noexcept {
		return audioBuffer_;
	}

	void load(gsl::span<channel_type> audio) override {
		audioBuffer_ = std::move(audio);
	}

	void setSampleRate(int r) noexcept {
		sampleRate_ = r;
	}

	void setChannels(int c) noexcept {
		channels_ = c;
	}

	void reset() override {
		log_.insert("reset ");
	}

	auto &log() const {
		return log_;
	}

	auto audioFrameReader() const noexcept {
		return audioFrameReader_;
	}

	auto audioFrameProcessor() const noexcept {
		return audioFrameProcessor_;
	}

	void setReader(std::shared_ptr<AudioFrameReader> r) override {
		audioFrameReader_ = std::move(r);
		log_.insert("setReader ");
	}

	void setProcessor(std::shared_ptr<AudioFrameProcessor> p) override {
		audioFrameProcessor_ = std::move(p);
		log_.insert("setProcessor ");
	}
};
