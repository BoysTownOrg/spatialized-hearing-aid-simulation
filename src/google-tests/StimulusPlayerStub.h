#pragma once

#include <recognition-test/StimulusPlayer.h>

class StimulusPlayerStub : public StimulusPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	std::vector<int> preferredProcessingSizes_{};
	PlayRequest request_{};
	std::string errorMessage{};
	bool failOnPlay_{};
	bool playing_{};
	bool playCalled_{};
	bool stopped_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void setPreferredProcessingSizes(std::vector<int> v) {
		preferredProcessingSizes_ = std::move(v);
	}

	const PlayRequest &request() const {
		return request_;
	}

	void failOnPlay() {
		failOnPlay_ = true;
	}

	void dontFailOnPlay() {
		failOnPlay_ = false;
	}

	void play(PlayRequest request) override {
		request_ = std::move(request);
		playCalled_ = true;
		if (failOnPlay_)
			throw RequestFailure{ errorMessage };
	}

	void setAudioDeviceDescriptions(std::vector<std::string> v) {
		audioDeviceDescriptions_ = std::move(v);
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
	}

	void setPlaying() {
		playing_ = true;
	}

	bool isPlaying() override {
		return playing_;
	}

	bool playCalled() {
		return playCalled_;
	}

	bool stopped() const {
		return stopped_;
	}

	void stop() override {
		stopped_ = true;
	}
};

class RequestFailingStimulusPlayer : public StimulusPlayer {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void play(PlayRequest) override {
		throw RequestFailure{ errorMessage };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	bool isPlaying() override { return {}; }
	void stop() override {}
};