#include "PortAudioDevice.h"
#include <gsl/gsl>

PortAudioDevice::PortAudioDevice() {
	lastError = Pa_Initialize();
}

void PortAudioDevice::openStream(StreamParameters parameters) {
	PaStreamParameters outputParameters{};
	outputParameters.device = parameters.deviceIndex;
	outputParameters.channelCount = parameters.channels;
	outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
	lastError = Pa_OpenStream(
		&stream,
		nullptr,
		&outputParameters,
		parameters.sampleRate,
		parameters.framesPerBuffer,
		paNoFlag,
		audioCallback,
		this
	);
}

int PortAudioDevice::audioCallback(
	const void *,
	void *output,
	const unsigned long frames,
	const PaStreamCallbackTimeInfo *,
	PaStreamCallbackFlags,
	void *self_
) {
	auto self = static_cast<PortAudioDevice *>(self_);
	self->controller->fillStreamBuffer(output, gsl::narrow<int>(frames));
	return self->callbackResult;
}

int PortAudioDevice::count() {
	return Pa_GetDeviceCount();
}

std::string PortAudioDevice::description(int i) {
	return Pa_GetDeviceInfo(i)->name;
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

bool PortAudioDevice::streaming() {
	return Pa_IsStreamActive(stream) == 1;
}

void PortAudioDevice::setCallbackResultToComplete() {
	callbackResult = paComplete;
}

void PortAudioDevice::setCallbackResultToContinue() {
	callbackResult = paContinue;
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

PortAudioDevice::~PortAudioDevice() noexcept {
	Pa_Terminate();
}
