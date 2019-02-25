#pragma once

#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/AudioProcessingLoader.h>

class FakeAudioProcessingLoader : public AudioProcessingLoader {
	ArgumentCollection<std::vector<std::vector<float>>> audio_{};
	int loadCount_{};
	int loadCompleteThreshold_{1};
public:
	void setLoadCompleteThreshold(int x) {
		loadCompleteThreshold_ = x;
	}

	bool complete() override {
		return loadCount_ >= loadCompleteThreshold_;
	}

	void load(gsl::span<channel_type> audio) override {
		std::vector<std::vector<float>> stored(audio.size());
		for (int i = 0; i < audio.size(); ++i)
			for (auto x : audio.at(i))
				stored.at(i).push_back(x);
		audio_.push_back(std::move(stored));
		++loadCount_;
	}

	auto audio() const {
		return audio_;
	}

	void reset() override {}
};