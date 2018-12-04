#include "PortAudioDevice.h"

PortAudioDevice::PortAudioDevice() {
	lastError = Pa_Initialize();
}

void PortAudioDevice::openStream(StreamParameters parameters) {
	PaStreamParameters outputParameters;
	outputParameters.device = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultOutputDevice;
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

void PortAudioDevice::closeStream() {
	lastError = Pa_CloseStream(stream);
}

void PortAudioDevice::startStream() {
	lastError = Pa_StartStream(stream);
}

void PortAudioDevice::stopStream() {
	lastError = Pa_StopStream(stream);
}

bool PortAudioDevice::streaming() const {
	return Pa_IsStreamActive(stream) == 1;
}

void PortAudioDevice::setCallbackResultToComplete() {
	callbackResult = paComplete;
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

PortAudioDevice::~PortAudioDevice() {
	Pa_Terminate();
}
