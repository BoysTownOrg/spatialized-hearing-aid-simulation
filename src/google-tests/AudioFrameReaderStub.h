#pragma once

#include <audio-stream-processing/AudioFrameReader.h>

class AudioFrameReaderStub : public AudioFrameReader {
	int _frameCount{};
	int _sampleRate{};
	float **_channels{};
	bool _complete{};
public:
	const float * const * channels() const {
		return _channels;
	}

	int frameCount() const {
		return _frameCount;
	}

	void read(float **channels, int frameCount) override {
		_channels = channels;
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
};