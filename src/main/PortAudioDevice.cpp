#include "PortAudioDevice.h"
#include <pa_asio.h>

PortAudioDevice::PortAudioDevice(Parameters parameters)
{
	PaStreamParameters outputParameters;
	outputParameters.device = getAsioDeviceIndex();
	outputParameters.channelCount = parameters.channels.size();
	outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
	PaAsioStreamInfo streamInfo;
	streamInfo.size = sizeof(PaAsioStreamInfo);
	streamInfo.version = 1;
	streamInfo.hostApiType = paASIO;
	streamInfo.flags = paAsioUseChannelSelectors;
	streamInfo.channelSelectors = &parameters.channels[0];
	outputParameters.hostApiSpecificStreamInfo = &streamInfo;
	lastError = Pa_OpenStream(
		&stream,
		nullptr,
		&outputParameters,
		parameters.sampleRate,
		parameters.framesPerBuffer,
		paNoFlag,
		audioCallback,
		this);
}

PaDeviceIndex PortAudioDevice::getAsioDeviceIndex() const
{
	const auto deviceCount = Pa_GetDeviceCount();
	for (PaDeviceIndex index = 0; index < deviceCount; ++index)
		if (isAsioDriven(index))
			return index;
	return -1;
}

bool PortAudioDevice::isAsioDriven(PaDeviceIndex index) const
{
	return
		Pa_GetDeviceInfo(index)->hostApi ==
		Pa_HostApiTypeIdToHostApiIndex(paASIO);
}

int PortAudioDevice::audioCallback(
	const void *,
	void *output,
	const unsigned long frames,
	const PaStreamCallbackTimeInfo *,
	PaStreamCallbackFlags,
	void *streamObject)
{
	static_cast<PortAudioDevice *>(streamObject)->controller->fillStreamBuffer(
		output,
		frames);
	return paContinue;
}

bool PortAudioDevice::failed() {
	return lastError != paNoError;
}

std::string PortAudioDevice::errorMessage() {
	return Pa_GetErrorText(lastError);
}

void PortAudioDevice::setController(AudioDeviceController *c) {
	controller = c;
}

void PortAudioDevice::startStream() {
	lastError = Pa_StartStream(stream);
}

void PortAudioDevice::stopStream() {
	lastError = Pa_StopStream(stream);
}

PortAudioDevice::~PortAudioDevice() {
	Pa_CloseStream(stream);
}
