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
	void setIsStreaming() {
		_streaming = true;
	}
};

class MockAudioDeviceFactory : public AudioDeviceFactory {
	AudioDevice::Parameters _parameters{};
	std::shared_ptr<AudioDevice> device;
	bool _makeCalled{};
public:
	explicit MockAudioDeviceFactory(
		std::shared_ptr<AudioDevice> device =
		std::make_shared<MockAudioDevice>()
	) :
		device{ std::move(device) } {}

	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters p) override {
		_makeCalled = true;
		_parameters = p;
		return device;
	}
	const AudioDevice::Parameters &parameters() const {
		return _parameters;
	}
	bool makeCalled() const {
		return _makeCalled;
	}
};