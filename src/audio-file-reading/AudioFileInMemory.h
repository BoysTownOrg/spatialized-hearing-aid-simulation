#pragma once

#include "audio-file-reading-exports.h"
#include <audio-file-reading/AudioFileReader.h>
#include <common-includes/RuntimeError.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <vector>

class AudioFileInMemory : public AudioFrameReader {
	using size_type = std::vector<float>::size_type;
	std::vector<float> buffer;
	int channelCount;
	size_type head = 0;
public:
	RUNTIME_ERROR(FileError);
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		AudioFileReader &reader
	);
	AUDIO_FILE_READING_API void read(float ** channels, int frameCount) override;
};

class AudioFileInMemoryFactory : public AudioFrameReaderFactory {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	explicit AudioFileInMemoryFactory(
		std::shared_ptr<AudioFileReaderFactory> factory
	) :
		factory{ std::move(factory) } {}

	std::shared_ptr<AudioFrameReader> make(std::string) override {
		try {
			auto reader = factory->make("");
			return std::make_shared<AudioFileInMemory>(*reader);
		}
		catch (const AudioFileInMemory::FileError &e) {
			throw FileError{ e.what() };
		}
	}
};