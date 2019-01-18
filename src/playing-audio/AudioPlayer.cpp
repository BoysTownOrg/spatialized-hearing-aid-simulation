#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	AudioDevice *device,
	AudioProcessor *processorFactory
) :
	device{ device },
	processor{ processorFactory }
{
	if (device->failed())
		throw DeviceFailure{ device->errorMessage() };
	device->setController(this);
}

void AudioPlayer::initialize(Initialization request) {
	AudioProcessor::Initialization initialization;
	initialization.attack_ms = request.attack_ms;
	initialization.release_ms = request.release_ms;
	initialization.brirFilePath = request.brirFilePath;
	initialization.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	initialization.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	initialization.chunkSize = request.chunkSize;
	initialization.windowSize = request.windowSize;
	initialization.max_dB_Spl = request.max_dB_Spl;
	try {
		processor->initialize(initialization);
	}
	catch (const AudioProcessor::InitializationFailure &e) {
		throw InitializationFailure{ e.what() };
	}
	framesPerBuffer_ = request.chunkSize;
}

void AudioPlayer::play(PlayRequest request) {
	if (device->streaming())
		return;
	
	try {
		AudioProcessor::Preparation p;
		p.audioFilePath = request.audioFilePath;
		p.level_dB_Spl = request.level_dB_Spl;
		processor->prepare(p);
	}
	catch (const AudioProcessor::PreparationFailure &e) {
		throw RequestFailure{ e.what() };
	}
	
	audio.resize(processor->channels());

	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = processor->sampleRate();
	streaming.channels = processor->channels();
	streaming.framesPerBuffer = framesPerBuffer_;
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			streaming.deviceIndex = i;

	device->closeStream();
	device->openStream(streaming);
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
	device->setCallbackResultToContinue();
	device->startStream();
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
	processor->process(audio);
	if (processor->complete())
		device->setCallbackResultToComplete();
}

std::vector<std::string> AudioPlayer::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}

bool AudioPlayer::isPlaying() {
	return device->streaming();
}

std::vector<int> AudioPlayer::preferredProcessingSizes() {
	return processor->preferredProcessingSizes();
}
