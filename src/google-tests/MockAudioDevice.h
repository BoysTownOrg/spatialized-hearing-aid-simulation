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
	bool streaming() const override {
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

class MockAudioDeviceFactory : public AudioDeviceFactory {
	AudioDevice::Parameters _parameters{};
	std::shared_ptr<AudioDevice> device;
	int _makeCalls{};
public:
	explicit MockAudioDeviceFactory(
		std::shared_ptr<AudioDevice> device =
		std::make_shared<MockAudioDevice>()
	) :
		device{ std::move(device) } {}

	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters p) override {
		++_makeCalls;
		_parameters = p;
		return device;
	}
	const AudioDevice::Parameters &parameters() const {
		return _parameters;
	}
	int makeCalls() const {
		return _makeCalls;
	}
};