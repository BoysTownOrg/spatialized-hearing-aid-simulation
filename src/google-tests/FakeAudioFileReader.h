#pragma once

#include <audio-file-reading/AudioFileReader.h>
#include <vector>

class FakeAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	std::string _errorMessage{};
	int _channels;
	int _sampleRate{};
	bool _failed{};
public:
	explicit FakeAudioFileReader(
		std::vector<float> contents = {},
		int _channels = 1
	) :
		contents{ std::move(contents) },
		_channels(_channels) {}

	void setChannels(int c) {
		_channels = c;
	}

	long long frames() override {
		return _channels == 0 ? 0 : contents.size() / _channels;
	}

	int channels() override {
		return _channels;
	}

	void readFrames(float *x, long long n) override {
		if (contents.size() == 0)
			return;
		std::memcpy(
			x,
			&contents[0],
			static_cast<std::size_t>(n) * sizeof(float) * _channels);
	}

	void readFrames(double *x, long long n) override {
		if (contents.size() == 0)
			return;
		for (int i = 0; i < n; ++i)
			x[i] = contents[i];
	}

	void fail() {
		_failed = true;
	}

	void setErrorMessage(std::string s) {
		_errorMessage = s;
	}

	bool failed() const override {
		return _failed;
	}

	std::string errorMessage() const override {
		return _errorMessage;
	}

	void setSampleRate(int s) {
		_sampleRate = s;
	}

	int sampleRate() override {
		return _sampleRate;
	}
};

class FakeAudioFileReaderFactory : public AudioFileReaderFactory {
	std::string _filePath{};
	std::shared_ptr<AudioFileReader> reader;
public:
	explicit FakeAudioFileReaderFactory(
		std::shared_ptr<AudioFileReader> reader =
			std::make_shared<FakeAudioFileReader>()
	) :
		reader{ std::move(reader) } {}

	std::shared_ptr<AudioFileReader> make(std::string filePath) override {
		_filePath = filePath;
		return reader;
	}

	std::string filePath() const {
		return _filePath;
	}
};