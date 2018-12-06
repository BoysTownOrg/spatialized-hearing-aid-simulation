#pragma once

#include "FakeAudioFileReader.h"
#include <audio-stream-processing/AudioFrameReader.h>

class FakeAudioFrameReader : public AudioFrameReader {
	std::shared_ptr<AudioFileReader> reader;
	long long frameCounter{};
public:
	explicit FakeAudioFrameReader(
		std::shared_ptr<AudioFileReader> reader =
			std::make_shared<FakeAudioFileReader>()
	) :
		reader{ std::move(reader) } {}

	void read(float ** audio, int frames) override {
		for (int i = 0; i < reader->channels(); ++i)
			reader->readFrames(audio[i], frames);
		frameCounter += frames;
	}

	bool complete() const override {
		return frameCounter >= reader->frames();
	}

	int sampleRate() const override {
		return reader->sampleRate();
	}

	int channels() const override {
		return reader->channels();
	}

	long long frames() const override {
		return reader->frames();
	}
};