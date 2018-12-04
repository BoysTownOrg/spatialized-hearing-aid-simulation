#pragma once

#include <audio-device-control/AudioDeviceController.h>
#include <portaudio.h>

class PortAudioDevice : public AudioDevice {
	AudioDeviceController *controller{};
	PaStream *stream{};
	PaError lastError{};
	PaStreamCallbackResult callbackResult{};
public:
	PortAudioDevice();
	~PortAudioDevice();
	PortAudioDevice(const PortAudioDevice &) = delete;
	PortAudioDevice(PortAudioDevice &&) = delete;
	PortAudioDevice &operator=(const PortAudioDevice &) = delete;
	PortAudioDevice &operator=(PortAudioDevice &&) = delete;
	void setController(AudioDeviceController *) override;
	void startStream() override;
	void stopStream() override;
	bool failed() override;
	std::string errorMessage() override;
	bool streaming() const override;
	void setCallbackResultToComplete() override;
	void openStream(Parameters parameters) override;
	void closeStream() override;
private:
	static int audioCallback(
		const void *,
		void *,
		unsigned long,
		const PaStreamCallbackTimeInfo *,
		PaStreamCallbackFlags,
		void *);
};

class PortAudioDeviceFactory : public AudioDeviceFactory {
	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters) override {
		return std::make_shared<PortAudioDevice>();
	}
};
