#pragma once

#include <audio-file-reading/AudioFileReader.h>
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
	) :
		contents{ std::move(contents) } {}

	void setChannels(int c) {
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
			x[i] = contents[i];
	}

	void fail() {
		failed_ = true;
	}

	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	bool failed() const override {
		return failed_;
	}

	std::string errorMessage() const override {
		return errorMessage_;
	}

	void setSampleRate(int s) {
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
	) :
		reader{ std::move(reader) } {}

	std::shared_ptr<AudioFileReader> make(std::string filePath) override {
		filePath_ = filePath;
		return reader;
	}

	std::string filePath() const {
		return filePath_;
	}
};