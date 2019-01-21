#pragma once

#include <playing-audio/AudioDevice.h>
#include <gsl/gsl>
#include <vector>

class AudioDeviceStub : public AudioDevice {
	std::vector<std::string> _descriptions{};
	std::string errorMessage_{};
	std::string _streamLog{};
	std::string _callbackLog{};
	StreamParameters _streamParameters{};
	AudioDeviceController *_controller{};
	bool _streaming{};
	bool failed_{};
	bool _setCallbackResultToCompleteCalled{};
	bool complete_{};
public:
	const AudioDeviceController *controller() const {
		return _controller;
	}

	void setController(AudioDeviceController *c) override {
		_controller = c;
	}

	bool streaming() override {
		return _streaming;
	}

	void setStreaming() {
		_streaming = true;
	}

	void startStream() override {
		_streamLog += "start ";
		_callbackLog += "start ";
	}

	void stopStream() override {
	}

	void fillStreamBuffer(void *x, int n) {
		_controller->fillStreamBuffer(x, n);
	}

	void fail() {
		failed_ = true;
	}

	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}

	bool setCallbackResultToCompleteCalled() const {
		return _setCallbackResultToCompleteCalled;
	}

	void setCallbackResultToComplete() override {
		_setCallbackResultToCompleteCalled = true;
		complete_ = true;
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

	void setCallbackResultToContinue() override {
		_callbackLog += "setCallbackResultToContinue ";
		complete_ = false;
	}

	void setDescriptions(std::vector<std::string> d) {
		_descriptions = std::move(d);
	}

	std::string description(int i) override {
		return _descriptions.at(i);
	}

	int count() override {
		return gsl::narrow_cast<int>(_descriptions.size());
	}

	std::string callbackLog() const {
		return _callbackLog;
	}

	bool complete() const {
		return complete_;
	}
};