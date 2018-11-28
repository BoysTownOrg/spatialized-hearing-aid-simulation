#include <playing-audio/AudioPlayerAdapter.h>
#include <gtest/gtest.h>

class MockAudioDeviceFactory : public AudioDeviceFactory {
	AudioDevice::Parameters _parameters{};
public:
	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters p) override {
		_parameters = p;
		return std::shared_ptr<AudioDevice>();
	}
	int framesPerBuffer() const {
		return _parameters.framesPerBuffer;
	}
	int sampleRate() const {
		return _parameters.sampleRate;
	}
};

class AudioPlayerAdapterTestCase : public ::testing::TestCase {};

TEST(AudioPlayerAdapterTestCase, makePassesDeviceParametersToFactory) {
	const auto factory = std::make_shared<MockAudioDeviceFactory>();
	AudioPlayerAdapterFactory adapter{ factory };
	AudioPlayer::Parameters p{};
	p.forAudioDevice.framesPerBuffer = 1;
	p.forAudioDevice.sampleRate = 2;
	adapter.make(p);
	EXPECT_EQ(1, factory->framesPerBuffer());
	EXPECT_EQ(2, factory->sampleRate());
}