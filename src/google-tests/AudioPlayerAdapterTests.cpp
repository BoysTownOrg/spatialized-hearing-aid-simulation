#include <playing-audio/AudioPlayerAdapter.h>
#include <gtest/gtest.h>

class AudioPlayerAdapterTestCase : public ::testing::TestCase {};

TEST(AudioPlayerAdapterTestCase, tbd) {
	AudioPlayerAdapterFactory adapter{};
	AudioPlayer::Parameters p{};
	adapter.make(p);
}