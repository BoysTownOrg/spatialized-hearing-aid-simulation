#pragma once

#include <audio-file-reading-writing/AudioFile.h>
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

class FakeAudioFileFactory : public AudioFileFactory {
	std::string filePathForReading_{};
	std::string filePathForWriting_{};
	std::shared_ptr<AudioFileReader> reader;
	std::shared_ptr<AudioFileWriter> writer;
public:
	explicit FakeAudioFileFactory(
		std::shared_ptr<AudioFileReader> reader
	) noexcept :
		reader{ std::move(reader) } {}

	explicit FakeAudioFileFactory(
		std::shared_ptr<AudioFileWriter> writer
	) noexcept :
		writer{ std::move(writer) } {}

	std::shared_ptr<AudioFileReader> makeReader(std::string filePath) override {
		filePathForReading_ = std::move(filePath);
		return reader;
	}

	std::shared_ptr<AudioFileWriter> makeWriterUsingLastReaderFormat(std::string filePath) override {
		filePathForWriting_ = std::move(filePath);
		return writer;
	}

	auto filePathForReading() const {
		return filePathForReading_;
	}

	auto filePathForWriting() const {
		return filePathForWriting_;
	}
};