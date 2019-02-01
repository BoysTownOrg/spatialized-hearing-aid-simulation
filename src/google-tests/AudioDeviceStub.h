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
	auto controller() const noexcept {
		return _controller;
	}

	void setController(AudioDeviceController *c) override {
		_controller = c;
	}

	bool streaming() override {
		return _streaming;
	}

	void setStreaming() noexcept {
		_streaming = true;
	}

	void startStream() override {
		_streamLog += "start ";
		_callbackLog += "start ";
	}

	void stopStream() override {
		_streamLog += "stop ";
	}

	void fillStreamBuffer(void *x, int n) {
		_controller->fillStreamBuffer(x, n);
	}

	void fail() noexcept {
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

	auto setCallbackResultToCompleteCalled() const noexcept {
		return _setCallbackResultToCompleteCalled;
	}

	void setCallbackResultToComplete() override {
		_setCallbackResultToCompleteCalled = true;
		complete_ = true;
	}

	auto streamLog() const {
		return _streamLog;
	}

	void openStream(StreamParameters p) override {
		_streamParameters = std::move(p);
		_streamLog += "open ";
	}

	void closeStream() override {
		_streamLog += "close ";
	}

	auto streamParameters() const noexcept {
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

	auto callbackLog() const {
		return _callbackLog;
	}

	bool complete() const {
		return complete_;
	}
};

class FailsToOpenStream : public AudioDevice {
	std::string errorMessage_{};
	bool failed_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	void openStream(StreamParameters) override {
		failed_ = true;
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}

	void setController(AudioDeviceController *) override {}
	void startStream() override {}
	void stopStream() override {}
	bool streaming() override { return {}; }
	void setCallbackResultToComplete() override {}
	void setCallbackResultToContinue() override {}
	void closeStream() override {}
	int count() override { return {}; }
	std::string description(int) override { return {}; }
};