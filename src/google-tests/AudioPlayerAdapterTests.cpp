#include <playing-audio/AudioPlayerAdapter.h>
#include <gtest/gtest.h>

class AudioPlayerAdapterTestCase : public ::testing::TestCase {};

TEST(AudioPlayerAdapterTestCase, tbd) {
	const auto factory = std::make_shared<MockAudioDeviceFactory>();
	AudioPlayerAdapterFactory adapter{ factory };
	AudioPlayer::Parameters p{};
	p.forAudioDevice.framesPerBuffer = 1;
	p.forAudioDevice.sampleRate = 2;
	adapter.make(p);
	EXPECT_EQ(1, factory->framesPerBuffer());
	EXPECT_EQ(2, factory->sampleRate());
}