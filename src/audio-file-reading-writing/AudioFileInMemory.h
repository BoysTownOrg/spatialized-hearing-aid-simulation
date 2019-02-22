#pragma once

#include "AudioFileReader.h"
#include "audio-file-reading-writing-exports.h"
#include <spatialized-hearing-aid-simulation/AudioFrameReader.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class AudioFileInMemory : public AudioFrameReader {
	using buffer_type = std::vector<channel_type::element_type>;
	using size_type = buffer_type::size_type;
	buffer_type buffer;
	size_type head = 0;
	long long frames_;
	int channels_;
	int sampleRate_;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileInMemory(AudioFileReader &);
	RUNTIME_ERROR(FileError);
	AUDIO_FILE_READING_WRITING_API void read(gsl::span<channel_type> audio) override;
	AUDIO_FILE_READING_WRITING_API bool complete() override;
	AUDIO_FILE_READING_WRITING_API int sampleRate() override;
	AUDIO_FILE_READING_WRITING_API int channels() override;
	AUDIO_FILE_READING_WRITING_API long long frames() override;
	AUDIO_FILE_READING_WRITING_API void reset() override;
    AUDIO_FILE_READING_WRITING_API long long remainingFrames() override;
private:
	bool complete_();
	size_type remainingFrames_();
};

class AudioFileInMemoryFactory : public AudioFrameReaderFactory {
	AudioFileReaderFactory *factory;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileInMemoryFactory(
		AudioFileReaderFactory *
	) noexcept;
	AUDIO_FILE_READING_WRITING_API 
		std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};