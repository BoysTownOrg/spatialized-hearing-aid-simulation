#pragma once

#include "RefactoredAudioFrameProcessor.h"
#include "AudioProcessor.h"
#include "playing-audio-exports.h"
#include <common-includes/RuntimeError.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RmsComputer {
	std::vector<std::vector<float>> entireAudioFile;
public:
	explicit RmsComputer(AudioFrameReader &reader) :
		entireAudioFile{ reader.channels() }
	{
		std::vector<gsl::span<float>> pointers;
		for (auto &channel : entireAudioFile) {
			channel.resize(gsl::narrow<std::vector<float>::size_type>(reader.frames()));
			pointers.push_back({ channel });
		}
		reader.read(pointers);
	}

	double compute(int channel) {
		double squaredSum{};
		const auto channel_ = entireAudioFile.at(channel);
		for (const auto sample : channel_)
			squaredSum += sample * sample;
		return std::sqrt(squaredSum / channel_.size());
	}
};

class AudioProcessorImpl : public AudioProcessor {
	RefactoredAudioFrameProcessorFactory::Parameters processing{};
	std::shared_ptr<RefactoredAudioFrameProcessor> processor{};
	std::shared_ptr<AudioFrameReader> reader{};
	AudioFrameReaderFactory *readerFactory;
	RefactoredAudioFrameProcessorFactory *processorFactory;
	int paddedZeroes{};
public:
	AudioProcessorImpl(
		AudioFrameReaderFactory *readerFactory, 
		RefactoredAudioFrameProcessorFactory *processorFactory
	) :
		readerFactory{ readerFactory },
		processorFactory{ processorFactory } 
	{
	}

	PLAYING_AUDIO_API void initialize(Initialization initialization) override;
	PLAYING_AUDIO_API void prepare(Preparation p) override;
	PLAYING_AUDIO_API void process(gsl::span<gsl::span<float>> audio);
	PLAYING_AUDIO_API bool complete();
	PLAYING_AUDIO_API int channels();
	PLAYING_AUDIO_API int sampleRate();

private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<RefactoredAudioFrameProcessor> makeProcessor(
		RefactoredAudioFrameProcessorFactory::Parameters p
	);
};