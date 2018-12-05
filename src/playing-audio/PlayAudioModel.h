#pragma once

#include "playing-audio-exports.h"
#include "AudioDevice.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <audio-file-reading/AudioFileReader.h>
#include <presentation/Model.h>
#include <common-includes/RuntimeError.h>

class PlayAudioModel : public Model, public AudioDeviceController {
	std::shared_ptr<AudioDevice> device;
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory;
	std::shared_ptr<AudioFrameProcessorFactory> processorFactory;
	std::shared_ptr<AudioFrameReader> frameReader{};
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
public:
	RUNTIME_ERROR(DeviceFailure);
	PLAYING_AUDIO_API PlayAudioModel(
		std::shared_ptr<AudioDevice> device,
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory,
		std::shared_ptr<AudioFrameProcessorFactory> processorFactory
	);
	PLAYING_AUDIO_API void play(PlayRequest) override;
	PLAYING_AUDIO_API void fillStreamBuffer(void *channels, int frameCount) override;
	std::vector<std::string> audioDeviceDescriptions() override;
private:
	std::shared_ptr<AudioFileReader> makeAudioFileReader(std::string filePath);
	std::shared_ptr<AudioFrameReader> makeAudioFrameReader(std::string filePath);
};
