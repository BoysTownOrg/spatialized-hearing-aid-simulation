#include "PortAudioDevice.h"
#include <pa_asio.h>

PortAudioDevice::PortAudioDevice(Parameters parameters)
{
	Pa_Initialize();
	PaStreamParameters outputParameters;
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = parameters.channels.size();
	outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
	outputParameters.hostApiSpecificStreamInfo = nullptr;
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

int PortAudioDevice::audioCallback(
	const void *,
	void *output,
	const unsigned long frames,
	const PaStreamCallbackTimeInfo *,
	PaStreamCallbackFlags,
	void *streamObject)
{
	const auto self = static_cast<PortAudioDevice *>(streamObject);
	self->controller->fillStreamBuffer(output, frames);
	return self->callbackResult;
}

void PortAudioDevice::setCallbackResultToComplete() {
	callbackResult = paComplete;
}

bool PortAudioDevice::streaming() const {
	return Pa_IsStreamActive(stream);
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
	Pa_Terminate();
}
