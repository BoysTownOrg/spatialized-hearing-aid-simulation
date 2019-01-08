#include <playing-audio/AudioPlayer.h>
#include "AudioDeviceStub.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

class AudioPlayerTests : public ::testing::Test {
protected:
	AudioDeviceStub device{};
	AudioPlayer player{ &device };
};

TEST_F(AudioPlayerTests, constructorSetsItselfAsDeviceController) {
	EXPECT_EQ(&player, device.controller());
}

TEST_F(AudioPlayerTests, playFirstClosesStreamThenOpensThenStarts) {
	player.play({});
	assertEqual("close open start ", device.streamLog());
}