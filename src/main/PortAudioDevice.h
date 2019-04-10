#pragma once

#include <playing-audio/AudioDevice.h>
#include <portaudio.h>

class PortAudioDevice : public AudioDevice {
	AudioDeviceController *controller{};
	PaStream *stream{};
	PaError lastError{};
	PaStreamCallbackResult callbackResult{};
public:
	PortAudioDevice();
	~PortAudioDevice() noexcept override;
	PortAudioDevice(const PortAudioDevice &) = delete;
	PortAudioDevice(PortAudioDevice &&) = delete;
	PortAudioDevice &operator=(const PortAudioDevice &) = delete;
	PortAudioDevice &operator=(PortAudioDevice &&) = delete;
	void setController(AudioDeviceController *) override;
	void startStream() override;
	void stopStream() override;
	bool failed() override;
	std::string errorMessage() override;
	bool streaming() override;
	void setCallbackResultToComplete() override;
	void setCallbackResultToContinue() override;
	void openStream(StreamParameters parameters) override;
	void closeStream() override;
	int count() override;
	std::string description(int) override;
private:
	static int audioCallback(
		const void *,
		void *,
		unsigned long,
		const PaStreamCallbackTimeInfo *,
		PaStreamCallbackFlags,
		void *
	);
};
