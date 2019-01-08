#include <playing-audio/AudioPlayer.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include "AudioDeviceStub.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include "assert-utility.h"
#include <gtest/gtest.h>

class AudioPlayerFacade {
	AudioDeviceStub device{};
	AudioFrameReaderStubFactory readerFactory{};
	AudioFrameProcessorStubFactory processorFactory{};
	AudioPlayer player;
public:
	AudioPlayerFacade(AudioDevice *device) :
		player{
			device,
			&readerFactory,
			&processorFactory
		}
	{}

	AudioPlayerFacade(AudioFrameReaderFactory *readerFactory) :
		player{
			&device,
			readerFactory,
			&processorFactory
		}
	{}

	AudioPlayerFacade(AudioFrameProcessorFactory *processorFactory) :
		player{
			&device,
			&readerFactory,
			processorFactory
		}
	{}

	void play() {
		player.play({});
	}
};

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
			FAIL() << "Expected AudioPlayer::RequestFailure";
		}
		catch (const AudioPlayer::RequestFailure &e) {
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

TEST_F(AudioPlayerTests, audioDeviceDescriptionsReturnsDescriptions) {
	device.setDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, player.audioDeviceDescriptions());
}

class ReadsAOne : public AudioFrameReader {
	void read(gsl::span<gsl::span<float>> audio) override {
		for (const auto channel : audio)
			for (auto &x : channel)
				x = 1;
	}
	bool complete() const override {
		return false;
	}
	int sampleRate() const override {
		return 0;
	}
	int channels() const override {
		return 1;
	}
	long long frames() const override {
		return 0;
	}
	void reset() override {
	}
};

class AudioTimesTwo : public AudioFrameProcessor {
	void process(gsl::span<gsl::span<float>> audio) override {
		for (const auto channel : audio)
			for (auto &x : channel)
				x *= 2;
	}
};

TEST_F(AudioPlayerTests, fillBufferReadsThenProcesses) {
	readerFactory.setReader(std::make_shared<ReadsAOne>());
	processorFactory.setProcessor(std::make_shared<AudioTimesTwo>());
	player.play({});
	float x{};
	float *audio[] = { &x };
	device.fillStreamBuffer(audio, 1);
	EXPECT_EQ(2, x);
}

TEST_F(AudioPlayerTests, playPassesComputedRmsToProcessorFactory) {
	FakeAudioFileReader fake{
		std::vector<float>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		2
	};
	readerFactory.setReader(std::make_shared<AudioFileInMemory>(fake));
	player.play({});
	assertEqual(
		{ 
			std::sqrt((1*1 + 3*3 + 5*5 + 7*7 + 9*9) / 5),
			std::sqrt((2*2 + 4*4 + 6*6 + 8*8 + 10*10) / 5)
		}, 
		processorFactory.parameters().stimulusRms,
		1e-6
	);
}

TEST_F(AudioPlayerTests, playResetsReaderAfterComputingRms) {
	player.play({});
	EXPECT_TRUE(frameReader->readingLog().endsWith("reset "));
}

TEST(
	RecognitionTestModelOtherTests,
	constructorThrowsDeviceFailureWhenDeviceFailsToInitialize
) {
	AudioDeviceStub device{};
	device.fail();
	device.setErrorMessage("error.");
	try {
		AudioPlayerFacade player{ &device };
		FAIL() << "Expected RecognitionTestModel::RequestFailure";
	}
	catch (const AudioPlayer::DeviceFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(
	AudioPlayerOtherTests,
	playThrowsRequestFailureWhenReaderFactoryThrowsCreateError
) {
	ErrorAudioFrameReaderFactory factory{ "error." };
	AudioPlayerFacade player{ &factory };
	try {
		player.play();
		FAIL() << "Expected AudioPlayer::RequestFailure";
	}
	catch (const AudioPlayer::RequestFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(
	AudioPlayerOtherTests,
	playThrowsRequestFailureWhenProcessorFactoryThrowsCreateError
) {
	ErrorAudioFrameProcessorFactory factory{ "error." };
	AudioPlayerFacade player{ &factory };
	try {
		player.play();
		FAIL() << "Expected AudioPlayer::RequestFailure";
	}
	catch (const AudioPlayer::RequestFailure &e) {
		assertEqual("error.", e.what());
	}
}