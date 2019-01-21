#pragma once

#include "AudioFrameReader.h"
#include <memory>

#ifdef AUDIO_STREAM_PROCESSING_EXPORTS
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllimport)
#endif

class ChannelCopier : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
public:
	AUDIO_STREAM_PROCESSING_API explicit ChannelCopier(
		std::shared_ptr<AudioFrameReader>
	);
	AUDIO_STREAM_PROCESSING_API void read(gsl::span<gsl::span<float>> audio) override;
	AUDIO_STREAM_PROCESSING_API bool complete() const override;
	AUDIO_STREAM_PROCESSING_API int sampleRate() const override;
	AUDIO_STREAM_PROCESSING_API int channels() const override;
	AUDIO_STREAM_PROCESSING_API long long frames() const override;
	AUDIO_STREAM_PROCESSING_API void reset() override;

    // Inherited via AudioFrameReader
    virtual long long framesRemaining() override;
};

class ChannelCopierFactory : public AudioFrameReaderFactory {
	std::shared_ptr<AudioFrameReaderFactory> factory;
public:
	AUDIO_STREAM_PROCESSING_API explicit ChannelCopierFactory(
		std::shared_ptr<AudioFrameReaderFactory>
	);
	AUDIO_STREAM_PROCESSING_API std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};