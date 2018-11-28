#include <playing-audio/AudioPlayer.h>

class AudioPlayerAdapterFactory : public AudioPlayerFactory {
public:
	std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) override {
		return {};
	}
};

#include <gtest/gtest.h>

class AudioPlayerAdapterTestCase : public ::testing::TestCase {};

TEST(AudioPlayerAdapterTestCase, tbd) {
	AudioPlayerAdapterFactory adapter{};
	AudioPlayer::Parameters p{};
	adapter.make(p);
}