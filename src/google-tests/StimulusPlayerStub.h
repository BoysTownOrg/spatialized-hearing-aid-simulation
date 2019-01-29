#pragma once

#include <recognition-test/StimulusPlayer.h>

class StimulusPlayerStub : public StimulusPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	std::vector<int> preferredProcessingSizes_{};
	Preparation preparation_{};
	std::string errorMessage{};
	bool failOnPrepareToPlay_{};
	bool playing_{};
	bool prepareToPlayCalled_{};
	bool stopped_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void setPreferredProcessingSizes(std::vector<int> v) {
		preferredProcessingSizes_ = std::move(v);
	}

	const Preparation &preparation() const {
		return preparation_;
	}

	void failOnPrepareToPlay() {
		failOnPrepareToPlay_ = true;
	}

	void dontFailOnPrepareToPlay() {
		failOnPrepareToPlay_ = false;
	}

	void prepareToPlay(Preparation p) override {
		preparation_ = std::move(p);
		prepareToPlayCalled_ = true;
		if (failOnPrepareToPlay_)
			throw PreparationFailure{ errorMessage };
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

	bool prepareToPlayCalled() {
		return prepareToPlayCalled_;
	}

	bool stopped() const {
		return stopped_;
	}

	void stop() override {
		stopped_ = true;
	}

	void play() override {}
};

class RequestFailingStimulusPlayer : public StimulusPlayer {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void prepareToPlay(Preparation) override {
		throw PreparationFailure{ errorMessage };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	bool isPlaying() override { return {}; }
	void stop() override {}
	void play() override {}
};