#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	AudioDevice *device, 
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	device{ device },
	readerFactory{ readerFactory },
	processorFactory{ processorFactory }
{
	if (device->failed())
		throw DeviceFailure{ device->errorMessage() };
	device->setController(this);
}

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

void AudioPlayer::play(PlayRequest request) {
	if (device->streaming())
		return;
	
	frameReader = makeReader(request.audioFilePath);
	audio.resize(frameReader->channels());

	AudioFrameProcessorFactory::Parameters processing;
	processing.attack_ms = request.attack_ms;
	processing.release_ms = request.release_ms;
	processing.channels = frameReader->channels();
	processing.brirFilePath = request.brirFilePath;
	processing.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	processing.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	processing.sampleRate = frameReader->sampleRate();
	processing.chunkSize = request.chunkSize;
	processing.windowSize = request.windowSize;
	processing.max_dB_Spl = request.max_dB_Spl;
	const auto desiredRms = std::pow(10.0, (request.level_dB_Spl - request.max_dB_Spl) / 20.0);
	RmsComputer rms{ *frameReader };
	for (int i = 0; i < frameReader->channels(); ++i)
		processing.channelScalars.push_back(desiredRms / rms.compute(i));
	frameProcessor = makeProcessor(processing);

	frameReader->reset();

	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = frameReader->sampleRate();
	streaming.channels = frameReader->channels();
	streaming.framesPerBuffer = request.chunkSize;
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

std::shared_ptr<AudioFrameReader> AudioPlayer::makeReader(std::string filePath) {
	try {
		return readerFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> AudioPlayer::makeProcessor(
	AudioFrameProcessorFactory::Parameters p
) {
	try {
		return processorFactory->make(std::move(p));
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

void AudioPlayer::initialize(Initialization)
{
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
	frameReader->read(audio);
	if (frameReader->complete()) {
		for (auto channel : audio)
			for (auto &x : channel)
				x = 0;
		paddedZeroes += frames;
		if (paddedZeroes >= frameProcessor->groupDelay()) {
			device->setCallbackResultToComplete();
			paddedZeroes = 0;
		}
	}
	frameProcessor->process(audio);
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
