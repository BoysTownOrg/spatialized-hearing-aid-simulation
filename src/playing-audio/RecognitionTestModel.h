#pragma once

#include "playing-audio-exports.h"
#include "AudioDevice.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <audio-file-reading/AudioFileReader.h>
#include <presentation/Model.h>
#include <common-includes/RuntimeError.h>

class RecognitionTestModel : public Model, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	std::shared_ptr<AudioDevice> device;
	std::shared_ptr<AudioFrameReaderFactory> readerFactory;
	std::shared_ptr<AudioFrameProcessorFactory> processorFactory;
	std::shared_ptr<AudioFrameReader> frameReader{};
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
public:
	RUNTIME_ERROR(DeviceFailure);
	PLAYING_AUDIO_API RecognitionTestModel(
		std::shared_ptr<AudioDevice> device,
		std::shared_ptr<AudioFrameReaderFactory> readerFactory,
		std::shared_ptr<AudioFrameProcessorFactory> processorFactory
	);
	PLAYING_AUDIO_API void play(PlayRequest) override;
	void fillStreamBuffer(void *channels, int frames) override;
	PLAYING_AUDIO_API void initializeTest(TestParameters) override;
	void playTrial() override;
	std::vector<std::string> audioDeviceDescriptions() override;
private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameProcessor> makeProcessor(AudioFrameProcessorFactory::Parameters p);
};
