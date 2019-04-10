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
		const gsl::span<float> audio{ x, gsl::narrow<gsl::span<float>::index_type>(n * channels_) };
		std::copy(contents.begin(), contents.end(), audio.begin());
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

class FakeAudioFileWriter : public AudioFileWriter {
	std::vector<float> written_{};
	std::string errorMessage_{};
	int channels_{ 1 };
	bool failed_{};
public:
	void setChannels(int x) noexcept {
		channels_ = x;
	}

	void fail() noexcept {
		failed_ = true;
	}

	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	auto written() {
		return written_;
	}

	void writeFrames(float *x, long long n) override {
		const gsl::span<float> audio{ x, gsl::narrow<gsl::span<float>::index_type>(n * channels_) };
		std::copy(audio.begin(), audio.end(), std::back_inserter(written_));
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}
};

class FakeAudioFileFactory : public AudioFileFactory {
    AudioFileWriter::AudioFileFormat formatForWriting_{};
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

	std::shared_ptr<AudioFileWriter> makeWriter(
        std::string filePath,
        const AudioFileWriter::AudioFileFormat &format
    ) override {
		filePathForWriting_ = std::move(filePath);
        formatForWriting_ = format;
		return writer;
	}

	auto filePathForReading() const {
		return filePathForReading_;
	}

	auto filePathForWriting() const {
		return filePathForWriting_;
	}
 
    auto &formatForWriting() const {
        return formatForWriting_;
    }
};
