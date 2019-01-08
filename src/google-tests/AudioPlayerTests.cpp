#include <playing-audio/AudioPlayer.h>
#include "AudioDeviceStub.h"
#include "AudioFrameReaderStub.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

class AudioPlayerTests : public ::testing::Test {
protected:
	AudioDeviceStub device{};
	std::shared_ptr<AudioFrameReaderStub> frameReader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory readerFactory{ frameReader };
	AudioPlayer player{ &device, &readerFactory };

	void assertPlayThrowsDeviceFailureWithMessage(std::string errorMessage) {
		try {
			player.play({});
			FAIL() << "Expected AudioPlayer::DeviceFailure";
		}
		catch (const AudioPlayer::DeviceFailure &e) {
			assertEqual(errorMessage, e.what());
		}
	}
};

TEST_F(AudioPlayerTests, constructorSetsItselfAsDeviceController) {
	EXPECT_EQ(&player, device.controller());
}

TEST_F(AudioPlayerTests, playFirstClosesStreamThenOpensThenStarts) {
	player.play({});
	assertEqual("close open start ", device.streamLog());
}

TEST_F(
	AudioPlayerTests,
	playThrowsDeviceFailureWhenDeviceFails
) {
	device.fail();
	device.setErrorMessage("error.");
	assertPlayThrowsDeviceFailureWithMessage("error.");
}

TEST_F(AudioPlayerTests, playWhileStreamingDoesNotAlterStream) {
	device.setStreaming();
	player.play({});
	EXPECT_TRUE(device.streamLog().empty());
}

TEST_F(AudioPlayerTests, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	player.play({});
	device.fillStreamBuffer({}, {});
	EXPECT_FALSE(device.setCallbackResultToCompleteCalled());
	frameReader->setComplete();
	device.fillStreamBuffer({}, {});
	EXPECT_TRUE(device.setCallbackResultToCompleteCalled());
}