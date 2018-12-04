#pragma once

#include <playing-audio/PlayAudioModel.h>
#include <portaudio.h>

class PortAudioDevice : public AudioDevice {
	PlayAudioModel *controller{};
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
	void setController(PlayAudioModel *) override;
	void startStream() override;
	void stopStream() override;
	bool failed() override;
	std::string errorMessage() override;
	bool streaming() const override;
	void setCallbackResultToComplete() override;
	void openStream(StreamParameters parameters) override;
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
