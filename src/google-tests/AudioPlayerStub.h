#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/IAudioPlayer.h>

class AudioPlayerStub : public IAudioPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	Preparation preparation_{};
	LogString log_{};
	std::function<void(void)> callOnPlay_{ []() {} };
	AudioLoader *audioLoader_{};
	bool isPlaying_{};
	bool played_{};
	bool stopped_{};
public:
	void prepareToPlay(Preparation p) override {
		preparation_ = std::move(p);
		log_ += LogString{ "prepareToPlay " };
	}

	const auto &preparation() const noexcept {
		return preparation_;
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
	}

	void setAudioDeviceDescriptions(std::vector<std::string> v) {
		audioDeviceDescriptions_ = std::move(v);
	}

	auto played() const {
		return played_;
	}

	void play() override {
		played_ = true;
		callOnPlay_();
		log_ += LogString{ "play " };
	}

	void setPlaying() {
		isPlaying_ = true;
	}

	bool isPlaying() override {
		return isPlaying_;
	}

	auto stopped() const {
		return stopped_;
	}

	void setAudioLoader(AudioLoader *a) override {
		audioLoader_ = a;
	}

	auto audioLoader() const noexcept {
		return audioLoader_;
	}

	void callOnPlay(std::function<void(void)> f) {
		callOnPlay_ = f;
	}

	auto log() const {
		return log_;
	}
};

class PreparationFailingAudioPlayer : public IAudioPlayer {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void prepareToPlay(Preparation) override {
		throw PreparationFailure{ errorMessage };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	void play() override {}
	bool isPlaying() override { return {}; }
	void setAudioLoader(AudioLoader *) override {}
};