#pragma once

#include <playing-audio/AudioDevice.h>
#include <gsl/gsl>
#include <vector>

class AudioDeviceStub : public AudioDevice {
	std::vector<std::string> _descriptions{};
	std::string errorMessage_{};
	std::string streamLog_{};
	StreamParameters _streamParameters{};
	AudioDeviceController *_controller{};
	bool streaming_{};
	bool failed_{};
	bool setCallbackResultToCompleteCalled_{};
	bool setCallbackResultToContinueCalled_{};
	bool complete_{};
	bool streamStarted_{};
	bool streamStopped_{};
public:
	auto streamStopped() const noexcept {
		return streamStopped_;
	}

	auto streamStarted() const noexcept {
		return streamStarted_;
	}

	auto controller() const noexcept {
		return _controller;
	}

	void setController(AudioDeviceController *c) override {
		_controller = c;
	}

	bool streaming() override {
		return streaming_;
	}

	void setStreaming() noexcept {
		streaming_ = true;
	}

	void startStream() override {
		streamLog_ += "start ";
		streamStarted_ = true;
	}

	void stopStream() override {
		streamLog_ += "stop ";
		streamStopped_ = true;
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
		return setCallbackResultToCompleteCalled_;
	}

	auto setCallbackResultToContinueCalled() const noexcept {
		return setCallbackResultToContinueCalled_;
	}

	void setCallbackResultToComplete() override {
		setCallbackResultToCompleteCalled_ = true;
		complete_ = true;
	}

	auto streamLog() const {
		return streamLog_;
	}

	void openStream(StreamParameters p) override {
		_streamParameters = std::move(p);
		streamLog_ += "open ";
	}

	void closeStream() override {
		streamLog_ += "close ";
	}

	auto streamParameters() const noexcept {
		return _streamParameters;
	}

	void setCallbackResultToContinue() override {
		setCallbackResultToContinueCalled_ = true;
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