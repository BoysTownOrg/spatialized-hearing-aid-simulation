#pragma once

#include "LogString.h"
#include <audio-stream-processing/AudioFrameReader.h>

class AudioFrameReaderStub : public AudioFrameReader {
	LogString _readingLog{};
	long long _frames{};
	int _framesRequested{};
	int _sampleRate{};
	int _channels{};
	float **_audioBuffer{};
	bool _complete{};
public:
	const float * const *audioBuffer() const {
		return _audioBuffer;
	}

	int channels() const override {
		return _channels;
	}

	int framesRequested() const {
		return _framesRequested;
	}

	void read(gsl::span<float *> audio, int frames) override {
		_audioBuffer = audio.data();
		_framesRequested = frames;
		_readingLog += LogString{ "read " };
	}

	void setComplete() {
		_complete = true;
	}

	bool complete() const override {
		return _complete;
	}

	int sampleRate() const override {
		return _sampleRate;
	}

	void setSampleRate(int r) {
		_sampleRate = r;
	}

	void setChannels(int c) {
		_channels = c;
	}

	long long frames() const override {
		return _frames;
	}

	void setFrames(long long f) {
		_frames = f;
	}

	LogString readingLog() const {
		return _readingLog;
	}

	void reset() override {
		_readingLog += LogString{ "reset " };
	}
};

class AudioFrameReaderStubFactory : public AudioFrameReaderFactory {
	std::string _filePath{};
	std::shared_ptr<AudioFrameReader> reader;
public:
	explicit AudioFrameReaderStubFactory(
		std::shared_ptr<AudioFrameReader> reader =
			std::make_shared<AudioFrameReaderStub>()
	) :
		reader{ std::move(reader) } {}

	std::shared_ptr<AudioFrameReader> make(std::string filePath) override {
		_filePath = filePath;
		return reader;
	}

	std::string filePath() const {
		return _filePath;
	}
};

class ErrorAudioFrameReaderFactory : public AudioFrameReaderFactory {
	std::string errorMessage{};
public:
	explicit ErrorAudioFrameReaderFactory(
		std::string errorMessage
	) : 
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<AudioFrameReader> make(std::string) override {
		throw CreateError{ errorMessage };
	}
};