#pragma once

#include <audio-file-reading-writing/AudioFileReader.h>
#include <gsl/gsl>
#include <vector>

class FakeAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	std::string errorMessage_{};
	int channels_{ 1 };
	int sampleRate_{};
	bool failed_{};
public:
	explicit FakeAudioFileReader(
		std::vector<float> contents = {}
	) noexcept :
		contents{ std::move(contents) } {}

	void setContents(std::vector<float> c) {
		contents = std::move(c);
	}

	void setChannels(int c) noexcept {
		channels_ = c;
	}

	long long frames() override {
		return channels_ == 0 ? 0 : contents.size() / channels_;
	}

	int channels() override {
		return channels_;
	}

	void readFrames(float *x, long long n) override {
		for (int i = 0; i < n * channels_; ++i)
			x[i] = contents.at(i);
	}

	void fail() noexcept {
		failed_ = true;
	}

	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}

	void setSampleRate(int s) noexcept {
		sampleRate_ = s;
	}

	int sampleRate() override {
		return sampleRate_;
	}
};

class FakeAudioFileReaderFactory : public AudioFileReaderFactory {
	std::string filePath_{};
	std::shared_ptr<AudioFileReader> reader;
public:
	explicit FakeAudioFileReaderFactory(
		std::shared_ptr<AudioFileReader> reader =
			std::make_shared<FakeAudioFileReader>()
	) noexcept :
		reader{ std::move(reader) } {}

	std::shared_ptr<AudioFileReader> make(std::string filePath) override {
		filePath_ = std::move(filePath);
		return reader;
	}

	auto filePath() const {
		return filePath_;
	}
};