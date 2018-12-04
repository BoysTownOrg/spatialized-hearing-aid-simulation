#pragma once

#include <audio-device-control/AudioDeviceController.h>

class MockAudioDevice : public AudioDevice {
	std::string _errorMessage{};
	std::string _streamLog{};
	StreamParameters _streamParameters{};
	AudioDeviceController *_controller{};
	bool _streaming{};
	bool _failed{};
	bool _setCallbackResultToCompleteCalled{};
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
	bool setCallbackResultToCompleteCalled() const {
		return _setCallbackResultToCompleteCalled;
	}
	void setCallbackResultToComplete() override {
		_setCallbackResultToCompleteCalled = true;
	}
	std::string streamLog() const {
		return _streamLog;
	}
	void openStream(StreamParameters p) override {
		_streamParameters = p;
		_streamLog += "open ";
	}
	void closeStream() override {
		_streamLog += "close ";
	}
	const StreamParameters &streamParameters() const {
		return _streamParameters;
	}
};