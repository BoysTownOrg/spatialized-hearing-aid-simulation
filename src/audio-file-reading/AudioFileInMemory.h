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
	long long frames_;
	int channels_;
	int sampleRate_;
public:
	RUNTIME_ERROR(FileError);
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		AudioFileReader &reader
	);
	AUDIO_FILE_READING_API void read(gsl::span<gsl::span<float>> audio) override;
	AUDIO_FILE_READING_API bool complete() const override;
	AUDIO_FILE_READING_API int sampleRate() const override;
	AUDIO_FILE_READING_API int channels() const override;
	AUDIO_FILE_READING_API long long frames() const override;
	AUDIO_FILE_READING_API void reset() override;
};

class AudioFileInMemoryFactory : public AudioFrameReaderFactory {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	AUDIO_FILE_READING_API explicit AudioFileInMemoryFactory(
		std::shared_ptr<AudioFileReaderFactory> factory
	);
	AUDIO_FILE_READING_API std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};