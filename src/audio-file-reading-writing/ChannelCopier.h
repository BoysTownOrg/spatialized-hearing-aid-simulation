#pragma once

#include "audio-file-reading-writing-exports.h"
#include <spatialized-hearing-aid-simulation/AudioFrameReader.h>
#include <memory>

class ChannelCopier : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
public:
	AUDIO_FILE_READING_WRITING_API explicit ChannelCopier(
		std::shared_ptr<AudioFrameReader>
	) noexcept;
	AUDIO_FILE_READING_WRITING_API void read(gsl::span<channel_type> audio) override;
	AUDIO_FILE_READING_WRITING_API bool complete() override;
	AUDIO_FILE_READING_WRITING_API int sampleRate() override;
	AUDIO_FILE_READING_WRITING_API int channels() override;
	AUDIO_FILE_READING_WRITING_API long long frames() override;
	AUDIO_FILE_READING_WRITING_API void reset() override;
    long long remainingFrames() override;
private:
	void readAndCopyFirstChannel(gsl::span<channel_type> audio);
	void readAllChannels(gsl::span<channel_type> audio);
	bool mono();
};

class ChannelCopierFactory : public AudioFrameReaderFactory {
	AudioFrameReaderFactory *factory;
public:
	AUDIO_FILE_READING_WRITING_API explicit ChannelCopierFactory(
		AudioFrameReaderFactory *
	) noexcept;
	AUDIO_FILE_READING_WRITING_API 
		std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};