#pragma once

#include <common-includes/Interface.h>

class AudioReader {
public:
	INTERFACE_OPERATIONS(AudioReader);
	virtual void read(float **channels, int frameCount) = 0;
};

class AudioProcessor {
public:
	INTERFACE_OPERATIONS(AudioProcessor);
	virtual void process(float **channels, int frameCount) = 0;
};

#include <audio-device-control/AudioStream.h>
#include <memory>

class ProcessedAudioStream : public AudioStream {
	std::shared_ptr<AudioReader> reader;
	std::shared_ptr<AudioProcessor> processor;
public:
	ProcessedAudioStream(
		std::shared_ptr<AudioReader> reader,
		std::shared_ptr<AudioProcessor> processor
	) :
		reader{ std::move(reader) },
		processor{ std::move(processor) } {}

	void fillBuffer(float **channels, int) override {
		reader->read(channels, 0);
		processor->process(channels, 0);
	}
};

