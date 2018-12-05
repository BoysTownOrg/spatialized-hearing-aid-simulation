#pragma once

#include "audio-file-reading-exports.h"
#include "AudioFileReader.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class AudioFileInMemory : public AudioFrameReader {
	using buffer_type = std::vector<float>;
	using size_type = buffer_type::size_type;
	buffer_type buffer;
	size_type head = 0;
	int _channels;
	int _sampleRate;
public:
	RUNTIME_ERROR(FileError);
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		AudioFileReader &reader
	);
	AUDIO_FILE_READING_API void read(float ** channels, int frames) override;
	AUDIO_FILE_READING_API bool complete() const override;
	AUDIO_FILE_READING_API int sampleRate() const override;
	AUDIO_FILE_READING_API int channels() const override;
};

class AudioFileInMemoryFactory : public AudioFrameReaderFactory {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	explicit AudioFileInMemoryFactory(
		std::shared_ptr<AudioFileReaderFactory> factory
	);
	std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};