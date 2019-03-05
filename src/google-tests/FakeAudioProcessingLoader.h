#pragma once

#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/AudioProcessingLoader.h>

class AudioLoaderSpy : public AudioLoader {
	using buffer_type = std::vector<channel_type::element_type>;
	ArgumentCollection<std::vector<buffer_type>> audio_{};
	int loadCount_{};
	int loadCompleteThreshold_{1};
public:
	void setLoadCompleteThreshold(int x) noexcept {
		loadCompleteThreshold_ = x;
	}

	bool complete() override {
		return loadCount_ >= loadCompleteThreshold_;
	}

	void load(gsl::span<channel_type> audio) override {
		std::vector<buffer_type> stored{};
		for (auto channel : audio) {
			stored.push_back(buffer_type{});
			std::copy(channel.begin(), channel.end(), std::back_inserter(stored.back()));
		}
		audio_.push_back(std::move(stored));
		++loadCount_;
	}

	auto audio() const {
		return audio_;
	}
};

class FakeAudioLoader : public AudioLoader {
	using buffer_type = std::vector<channel_type::element_type>;
	buffer_type audioToLoad_{};
	buffer_type::size_type head{};
public:
	void setAudioToLoad(buffer_type x) {
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