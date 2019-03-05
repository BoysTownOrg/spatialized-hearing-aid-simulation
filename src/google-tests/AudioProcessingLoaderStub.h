#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/AudioProcessingLoader.h>

class AudioLoaderStub : public AudioLoader {
	LogString log_{};
	gsl::span<channel_type> audioBuffer_{};
	bool complete_{};
public:
	void setComplete() noexcept {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}

	void load(gsl::span<channel_type> audio) override {
		audioBuffer_ = std::move(audio);
	}

	auto audioBuffer() const noexcept {
		return audioBuffer_;
	}
};

class AudioProcessingLoaderStubFactory : public AudioProcessingLoaderFactory {
	std::shared_ptr<AudioFrameReader> reader_{};
	std::shared_ptr<AudioFrameProcessor> processor_{};
	std::shared_ptr<AudioLoader> loader;
public:
	explicit AudioProcessingLoaderStubFactory(
		std::shared_ptr<AudioLoader> loader =
			std::make_shared<AudioLoaderStub>()
	) noexcept :
		loader{ std::move(loader) } {}

	void setLoader(std::shared_ptr<AudioLoader> loader_) {
		loader = std::move(loader_);
	}

	std::shared_ptr<AudioLoader> make(
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