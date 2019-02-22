#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/AudioProcessingLoader.h>

class AudioProcessingLoaderStub : public AudioProcessingLoader {
	LogString log_{};
	gsl::span<channel_type> audioBuffer_{};
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

	auto &log() const noexcept {
		return log_;
	}
};

class AudioProcessingLoaderStubFactory : public AudioProcessingLoaderFactory {
	std::shared_ptr<AudioFrameReader> reader_{};
	std::shared_ptr<AudioFrameProcessor> processor_{};
	std::shared_ptr<AudioProcessingLoader> loader;
public:
	explicit AudioProcessingLoaderStubFactory(
		std::shared_ptr<AudioProcessingLoader> loader =
			std::make_shared<AudioProcessingLoaderStub>()
	) noexcept :
		loader{ std::move(loader) } {}

	std::shared_ptr<AudioProcessingLoader> make(
		std::shared_ptr<AudioFrameReader> r,
		std::shared_ptr<AudioFrameProcessor> p
	) override {
		reader_ = std::move(r);
		processor_ = std::move(p);
		return loader;
	}

	auto audioFrameReader() const {
		return reader_;
	}

	auto audioFrameProcessor() const {
		return processor_;
	}
};