#pragma once

#include "audio-stream-processing-exports.h"
#include "AudioFrameReader.h"
#include <memory>

class ChannelCopier : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
public:
	AUDIO_STREAM_PROCESSING_API explicit ChannelCopier(
		std::shared_ptr<AudioFrameReader>
	);
	AUDIO_STREAM_PROCESSING_API void read(gsl::span<float *> audio, int frames) override;
	AUDIO_STREAM_PROCESSING_API bool complete() const override;
	AUDIO_STREAM_PROCESSING_API int sampleRate() const override;
	AUDIO_STREAM_PROCESSING_API int channels() const override;
	AUDIO_STREAM_PROCESSING_API long long frames() const override;
	AUDIO_STREAM_PROCESSING_API void reset() override;
};

class ChannelCopierFactory : public AudioFrameReaderFactory {
	std::shared_ptr<AudioFrameReaderFactory> factory;
public:
	AUDIO_STREAM_PROCESSING_API explicit ChannelCopierFactory(
		std::shared_ptr<AudioFrameReaderFactory> factory
	);
	AUDIO_STREAM_PROCESSING_API std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};