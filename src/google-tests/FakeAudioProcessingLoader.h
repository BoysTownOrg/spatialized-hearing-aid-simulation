#pragma once

#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/AudioProcessingLoader.h>

class FakeAudioProcessingLoader : public AudioLoader {
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
};

class FakeAudioProcessingLoader2 : public AudioLoader {
	std::vector<float> audioToLoad_{};
	std::vector<float>::size_type head{};
public:
	void setAudioToLoad(std::vector<float> x) {
		audioToLoad_ = std::move(x);
	}

	bool complete() override {
		return head == audioToLoad_.size();
	}

	void load(gsl::span<channel_type> audio) override {
		for (auto channel : audio)
			for (auto &x : channel)
				x = audioToLoad_.at(head++);
	}
};