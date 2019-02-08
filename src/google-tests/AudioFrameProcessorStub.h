#pragma once

#include <spatialized-hearing-aid-simulation/AudioFrameProcessor.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	gsl::span<channel_type> audioBuffer_{};
	int groupDelay_{};
	bool complete_{};
public:
	auto audioBuffer() const noexcept {
		return audioBuffer_;
	}

	void process(gsl::span<channel_type> audio) override {
		audioBuffer_ = audio;
	}

	void setGroupDelay(int n) noexcept {
		groupDelay_ = n;
	}

	channel_type::index_type groupDelay() override {
		return groupDelay_;
	}

	void setComplete() noexcept {
		complete_ = true;
	}
};