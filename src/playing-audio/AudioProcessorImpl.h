#pragma once

#include "RefactoredAudioFrameProcessor.h"
#include "AudioProcessor.h"
#include "playing-audio-exports.h"
#include <audio-stream-processing/AudioFrameReader.h>

class AudioProcessorImpl : public AudioProcessor {
	AudioFrameProcessorFactory::Parameters processing{};
	std::shared_ptr<AudioFrameProcessor> processor{};
	std::shared_ptr<AudioFrameReader> reader{};
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
	int paddedZeroes{};
public:
	PLAYING_AUDIO_API AudioProcessorImpl(
		AudioFrameReaderFactory *readerFactory,
		AudioFrameProcessorFactory *processorFactory
	);

	PLAYING_AUDIO_API void initialize(Initialization initialization) override;
	PLAYING_AUDIO_API void prepare(Preparation p) override;
	PLAYING_AUDIO_API void process(gsl::span<gsl::span<float>> audio);
	PLAYING_AUDIO_API bool complete();
	PLAYING_AUDIO_API int channels();
	PLAYING_AUDIO_API int sampleRate();

private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameProcessor> makeProcessor(
		AudioFrameProcessorFactory::Parameters p
	);
};