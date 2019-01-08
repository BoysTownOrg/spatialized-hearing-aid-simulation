#include <playing-audio/AudioDevice.h>

class AudioPlayer : public AudioDeviceController {
public:
	AudioPlayer(AudioDevice *device) {
		device->setController(this);
	}

	void fillStreamBuffer(void * channels, int frames) override {
		channels;
		frames;
	}
};

#include "AudioDeviceStub.h"
#include <gtest/gtest.h>

class AudioPlayerTests : public ::testing::Test {
protected:
	AudioDeviceStub device{};
	AudioPlayer player{ &device };
};

TEST_F(AudioPlayerTests, constructorSetsItselfAsDeviceController) {
	EXPECT_EQ(&player, device.controller());
}