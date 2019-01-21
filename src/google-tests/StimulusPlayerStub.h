#pragma once

#include <recognition-test/StimulusPlayer.h>

class StimulusPlayerStub : public StimulusPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	std::vector<int> preferredProcessingSizes_{};
	Initialization initialization_{};
	PlayRequest request_{};
	bool playing_{};
	bool playCalled_{};
public:
	void setPreferredProcessingSizes(std::vector<int> v) {
		preferredProcessingSizes_ = std::move(v);
	}

	std::vector<int> preferredProcessingSizes() override {
		return preferredProcessingSizes_;
	}

	const Initialization &initialization() const {
		return initialization_;
	}

	const PlayRequest &request() const {
		return request_;
	}

	void play(PlayRequest request) override {
		request_ = std::move(request);
		playCalled_ = true;
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

	void initialize(Initialization i) override {
		initialization_ = std::move(i);
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
	void initialize(Initialization) override {}
	std::vector<int> preferredProcessingSizes() override { return {}; }
};

class InitializationFailingStimulusPlayer : public StimulusPlayer {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void initialize(Initialization) override {
		throw InitializationFailure{ errorMessage };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	bool isPlaying() override { return {}; }
	void play(PlayRequest) override {}
	std::vector<int> preferredProcessingSizes() override {
		return {};
	}
};