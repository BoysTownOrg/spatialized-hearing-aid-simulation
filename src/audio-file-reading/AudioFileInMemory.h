#pragma once

#include "AudioFileReader.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <common-includes/RuntimeError.h>
#include <vector>

#ifdef AUDIO_FILE_READING_EXPORTS
	#define AUDIO_FILE_READING_API __declspec(dllexport)
#else
	#define AUDIO_FILE_READING_API __declspec(dllimport)
#endif

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
	AUDIO_FILE_READING_API explicit AudioFileInMemory(AudioFileReader &);
	AUDIO_FILE_READING_API void read(gsl::span<channel_type> audio) override;
	AUDIO_FILE_READING_API bool complete() override;
	AUDIO_FILE_READING_API int sampleRate() override;
	AUDIO_FILE_READING_API int channels() override;
	AUDIO_FILE_READING_API long long frames() override;
	AUDIO_FILE_READING_API void reset() override;
    AUDIO_FILE_READING_API long long framesRemaining() override;
};

class AudioFileInMemoryFactory : public AudioFrameReaderFactory {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	AUDIO_FILE_READING_API explicit AudioFileInMemoryFactory(
		std::shared_ptr<AudioFileReaderFactory>
	);
	AUDIO_FILE_READING_API std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};