#pragma once

#include <recognition-test/StimulusPlayer.h>

class StimulusPlayerStub : public StimulusPlayer {
	bool playing_{};
	bool stopped_{};
public:
	void setPlaying() {
		playing_ = true;
	}

	bool isPlaying() override {
		return playing_;
	}

	bool stopped() const {
		return stopped_;
	}

	void stop() override {
		stopped_ = true;
	}

	void play() override {}
};