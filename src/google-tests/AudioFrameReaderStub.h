#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/AudioFrameReader.h>

class AudioFrameReaderStub : public AudioFrameReader {
	LogString readingLog_{};
	gsl::span<channel_type> audioBuffer_{};
	long long frames_{};
	int sampleRate_{};
	int channels_{};
    int remainingFrames_{};
	bool complete_{};
public:
    long long remainingFrames() override {
        return remainingFrames_;
    }

    void setRemainingFrames(int n) noexcept {
        remainingFrames_ = n;
    }

	auto audioBuffer() const noexcept {
		return audioBuffer_;
	}

	int channels() override {
		return channels_;
	}

	void read(gsl::span<channel_type> audio) override {
		audioBuffer_ = audio;
		readingLog_ += LogString{ "read " };
	}

	void setComplete() noexcept {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}

	int sampleRate() override {
		return sampleRate_;
	}

	void setSampleRate(int r) noexcept {
		sampleRate_ = r;
	}

	void setChannels(int c) noexcept {
		channels_ = c;
	}

	long long frames() override {
		return frames_;
	}

	void setFrames(long long f) noexcept {
		frames_ = f;
	}

	auto readingLog() const {
		return readingLog_;
	}

	void reset() override {
		readingLog_ += LogString{ "reset " };
	}

	void setIncomplete() noexcept {
		complete_ = false;
	}
};

class AudioFrameReaderStubFactory : public AudioFrameReaderFactory {
	std::string filePath_{};
	std::shared_ptr<AudioFrameReader> reader;
public:
	explicit AudioFrameReaderStubFactory(
		std::shared_ptr<AudioFrameReader> reader =
			std::make_shared<AudioFrameReaderStub>()
	) noexcept :
		reader{ std::move(reader) } {}

	void setReader(std::shared_ptr<AudioFrameReader> r) noexcept {
		reader = std::move(r);
	}

	std::shared_ptr<AudioFrameReader> make(std::string filePath) override {
		filePath_ = std::move(filePath);
		return reader;
	}

	auto filePath() const {
		return filePath_;
	}
};

class ErrorAudioFrameReaderFactory : public AudioFrameReaderFactory {
	std::string errorMessage{};
public:
	explicit ErrorAudioFrameReaderFactory(
		std::string errorMessage
	) noexcept : 
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<AudioFrameReader> make(std::string) override {
		throw CreateError{ errorMessage };
	}
};