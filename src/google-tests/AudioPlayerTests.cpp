#include <playing-audio/AudioPlayer.h>
#include "AudioDeviceStub.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

class AudioPlayerTests : public ::testing::Test {
protected:
	AudioDeviceStub device{};
	std::shared_ptr<AudioFrameReaderStub> frameReader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory readerFactory{ frameReader };
	std::shared_ptr<AudioFrameProcessorStub> processor = std::make_shared<AudioFrameProcessorStub>();
	AudioFrameProcessorStubFactory processorFactory{ processor };
	AudioPlayer player{ &device, &readerFactory, &processorFactory };

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

TEST_F(AudioPlayerTests, fillStreamBufferPassesAudio) {
	frameReader->setChannels(2);
	player.play({});
	float left{};
	float right{};
	float *x[]{ &left, &right };
	device.fillStreamBuffer(x, 1);
	EXPECT_EQ(&left, frameReader->audioBuffer()[0].data());
	EXPECT_EQ(&right, frameReader->audioBuffer()[1].data());
	EXPECT_EQ(1, frameReader->audioBuffer()[0].size());
	EXPECT_EQ(1, frameReader->audioBuffer()[1].size());
	EXPECT_EQ(&left, processor->audioBuffer()[0].data());
	EXPECT_EQ(&right, processor->audioBuffer()[1].data());
	EXPECT_EQ(1, processor->audioBuffer()[0].size());
	EXPECT_EQ(1, processor->audioBuffer()[1].size());
}

TEST_F(AudioPlayerTests, playPassesParametersToFactories) {
	StimulusPlayer::PlayRequest request;
	request.leftDslPrescriptionFilePath = "a";
	request.rightDslPrescriptionFilePath = "b";
	request.audioFilePath = "c";
	request.brirFilePath = "d";
	device.setDescriptions({ "alpha", "beta", "gamma", "lambda" });
	request.audioDevice = "gamma";
	request.level_dB_Spl = 1;
	request.attack_ms = 2;
	request.release_ms = 3;
	request.windowSize = 4;
	request.chunkSize = 5;
	frameReader->setChannels(6);
	frameReader->setSampleRate(7);
	player.play(request);
	assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
	assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
	assertEqual("c", readerFactory.filePath());
	assertEqual("d", processorFactory.parameters().brirFilePath);
	EXPECT_EQ(2, device.streamParameters().deviceIndex);
	EXPECT_EQ(1, processorFactory.parameters().level_dB_Spl);
	EXPECT_EQ(2, processorFactory.parameters().attack_ms);
	EXPECT_EQ(3, processorFactory.parameters().release_ms);
	EXPECT_EQ(4, processorFactory.parameters().windowSize);
	EXPECT_EQ(5, processorFactory.parameters().chunkSize);
	EXPECT_EQ(5, device.streamParameters().framesPerBuffer);
	EXPECT_EQ(6, device.streamParameters().channels);
	EXPECT_EQ(7, device.streamParameters().sampleRate);
}

TEST_F(AudioPlayerTests, playSetsCallbackResultToContinueBeforeStartingStream) {
	player.play({});
	assertEqual("setCallbackResultToContinue start ", device.callbackLog());
}