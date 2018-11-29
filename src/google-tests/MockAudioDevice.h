#pragma once

#include <audio-device-control/AudioDeviceController.h>

class MockAudioDevice : public AudioDevice {
	std::string _errorMessage{};
	AudioDeviceController *_controller{};
	bool _streaming{};
	bool _failed{};
public:
	const AudioDeviceController *controller() const {
		return _controller;
	}
	void setController(AudioDeviceController *c) override {
		_controller = c;
	}
	bool streaming() const {
		return _streaming;
	}
	void startStream() override {
		_streaming = true;
	}
	void stopStream() override {
		_streaming = false;
	}
	void fillStreamBuffer(void *x, int n) {
		_controller->fillStreamBuffer(x, n);
	}
	void setFailedTrue() {
		_failed = true;
	}
	void setErrorMessage(std::string s) {
		_errorMessage = s;
	}
	bool failed() override {
		return _failed;
	}
	std::string errorMessage() override {
		return _errorMessage;
	}
};