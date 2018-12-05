#pragma once

#include <audio-stream-processing/AudioFrameReader.h>

class AudioFrameReaderStub : public AudioFrameReader {
	int _frameCount{};
	int _sampleRate{};
	int _channels{};
	float **_audioBuffer{};
	bool _complete{};
public:
	const float * const * audioBuffer() const {
		return _audioBuffer;
	}

	int channels() const override {
		return _channels;
	}

	int frameCount() const {
		return _frameCount;
	}

	void read(float **audioBuffer, int frameCount) override {
		_audioBuffer = audioBuffer;
		_frameCount = frameCount;
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
};