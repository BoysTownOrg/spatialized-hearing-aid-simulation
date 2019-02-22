#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/AudioPlayer.h>

class AudioPlayerStub : public AudioPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	Preparation preparation_{};
	LogString log_{};
	std::function<void(void)> callOnPlay_{ []() {} };
	std::shared_ptr<AudioLoader> audioLoader_{};
	bool isPlaying_{};
	bool played_{};
	bool stopped_{};
public:
	void prepareToPlay(Preparation p) override {
		preparation_ = std::move(p);
		log_.insert("prepareToPlay ");
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

	auto played() const noexcept {
		return played_;
	}

	void play() override {
		played_ = true;
		callOnPlay_();
		log_.insert("play ");
	}

	void setPlaying() noexcept {
		isPlaying_ = true;
	}

	bool isPlaying() override {
		return isPlaying_;
	}

	void stop() override {
		stopped_ = true;
	}

	auto stopped() const noexcept {
		return stopped_;
	}

	void setAudioLoader(std::shared_ptr<AudioLoader> a) override {
		audioLoader_ = std::move(a);
	}

	auto audioLoader() const noexcept {
		return audioLoader_;
	}

	void callOnPlay(std::function<void(void)> f) {
		callOnPlay_ = f;
	}

	auto &log() const noexcept {
		return log_;
	}
};

class PreparationFailingAudioPlayer : public AudioPlayer {
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
	void stop() override {}
	bool isPlaying() override { return {}; }
	void setAudioLoader(std::shared_ptr<AudioLoader>) override {}
};