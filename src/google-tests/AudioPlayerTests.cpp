#include <playing-audio/AudioPlayer.h>
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