#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "AudioDeviceStub.h"
#include <playing-audio/PlayAudioModel.h>
#include <gtest/gtest.h>

class PlayAudioModelFacade {
	PlayAudioModel model;
public:
	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioDevice> device =
			std::make_shared<AudioDeviceStub>(),
		std::shared_ptr<AudioFrameProcessorFactory> processorFactory =
			std::make_shared<AudioFrameProcessorStubFactory>(),
		std::shared_ptr<AudioFrameReaderFactory> audioFactory =
			std::make_shared<AudioFrameReaderStubFactory>()
	) :
		model{ 
			std::move(device),
			std::move(audioFactory),
			std::move(processorFactory)
		} {}

	void play(PlayAudioModel::PlayRequest r = {}) {
		model.play(r);
	}

	const PlayAudioModel *get() const {
		return &model;
	}

	std::vector<std::string> audioDeviceDescriptions() {
		return model.audioDeviceDescriptions();
	}
};

class PlayAudioModelTestCase : public ::testing::TestCase {};

TEST(PlayAudioModelTestCase, constructorSetsItself) {
	const auto device = std::make_shared<AudioDeviceStub>();
	PlayAudioModelFacade model{ device };
	EXPECT_EQ(model.get(), device->controller());
}

static void assertDeviceFailureOnConstruction(
	std::shared_ptr<AudioDevice> device, 
	std::string message
) {
	try {
		PlayAudioModelFacade model{ device };
		FAIL() << "Expected PlayAudioModel::DeviceFailure";
	}
	catch (const PlayAudioModel::DeviceFailure &e) {
		assertEqual(message, e.what());
	}
}

TEST(
	PlayAudioModelTestCase,
	constructorThrowsDeviceFailureWhenDeviceFailsToInitialize)
{
	const auto device = std::make_shared<AudioDeviceStub>();
	device->fail();
	device->setErrorMessage("error.");
	assertDeviceFailureOnConstruction(device, "error.");
}

TEST(PlayAudioModelTestCase, playFirstClosesStreamThenOpensThenStarts) {
	const auto device = std::make_shared<AudioDeviceStub>();
	PlayAudioModelFacade model{ device };
	model.play();
	assertEqual("close open start ", device->streamLog());
}

TEST(
	PlayAudioModelTestCase,
	playThrowsRequestErrorWhenDeviceFailure)
{
	try {
		const auto device = std::make_shared<AudioDeviceStub>();
		PlayAudioModelFacade model{ device };
		device->fail();
		device->setErrorMessage("error.");
		model.play();
		FAIL() << "Expected PlayAudioModel::RequestFailure";
	}
	catch (const PlayAudioModel::RequestFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(PlayAudioModelTestCase, playWhileStreamingDoesNotAlterCurrentStream) {
	const auto device = std::make_shared<AudioDeviceStub>();
	PlayAudioModelFacade model{ device };
	device->setStreaming();
	model.play();
	EXPECT_TRUE(device->streamLog().empty());
}

TEST(PlayAudioModelTestCase, playPassesParametersToFactories) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	const auto audioFactory = std::make_shared<AudioFrameReaderStubFactory>(reader);
	const auto processorFactory = std::make_shared<AudioFrameProcessorStubFactory>();
	PlayAudioModelFacade model{
		device,
		processorFactory,
		audioFactory
	};
	device->setDescriptions({ "alpha", "beta", "gamma", "lambda" });
	reader->setSampleRate(48000);
	reader->setChannels(42);
	PlayAudioModel::PlayRequest request;
	request.leftDslPrescriptionFilePath = "a";
	request.rightDslPrescriptionFilePath = "b";
	request.audioFilePath = "c";
	request.brirFilePath = "d";
	request.audioDevice = "gamma";
	request.level_dB_Spl = 1;
	request.attack_ms = 2;
	request.release_ms = 3;
	request.windowSize = 4;
	request.chunkSize = 5;
	model.play(request);
	assertEqual("a", processorFactory->parameters().leftDslPrescriptionFilePath);
	assertEqual("b", processorFactory->parameters().rightDslPrescriptionFilePath);
	assertEqual("c", audioFactory->filePath());
	assertEqual("d", processorFactory->parameters().brirFilePath);
	EXPECT_EQ(1, processorFactory->parameters().level_dB_Spl);
	EXPECT_EQ(2, processorFactory->parameters().attack_ms);
	EXPECT_EQ(3, processorFactory->parameters().release_ms);
	EXPECT_EQ(4, processorFactory->parameters().windowSize);
	EXPECT_EQ(5, processorFactory->parameters().chunkSize);
	EXPECT_EQ(5, device->streamParameters().framesPerBuffer);
	EXPECT_EQ(48000, device->streamParameters().sampleRate);
	EXPECT_EQ(42, device->streamParameters().channels);
	EXPECT_EQ(2, device->streamParameters().deviceIndex);
}

TEST(PlayAudioModelTestCase, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	const auto audioFactory = std::make_shared<AudioFrameReaderStubFactory>(reader);
	PlayAudioModelFacade model{ device, std::make_shared<AudioFrameProcessorStubFactory>(), audioFactory };
	model.play();
	device->fillStreamBuffer(nullptr, 0);
	EXPECT_FALSE(device->setCallbackResultToCompleteCalled());
	reader->setComplete();
	device->fillStreamBuffer(nullptr, 0);
	EXPECT_TRUE(device->setCallbackResultToCompleteCalled());
}

TEST(PlayAudioModelTestCase, fillStreamBufferPassesAudio) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	const auto processor = std::make_shared<AudioFrameProcessorStub>();
	PlayAudioModelFacade model{ 
		device, 
		std::make_shared<AudioFrameProcessorStubFactory>(processor),
		std::make_shared<AudioFrameReaderStubFactory>(reader) 
	};
	model.play();
	float left{};
	float right{};
	float *x[]{ &left, &right };
	device->fillStreamBuffer(x, 1);
	EXPECT_EQ(x, reader->audioBuffer());
	EXPECT_EQ(1, reader->frames());
	EXPECT_EQ(x, processor->audioBuffer());
	EXPECT_EQ(1, processor->frames());
}

TEST(PlayAudioModelTestCase, playSetsCallbackResultToContinue) {
	const auto device = std::make_shared<AudioDeviceStub>();
	PlayAudioModelFacade model{ device };
	model.play();
	EXPECT_TRUE(device->setCallbackResultToContinueCalled());
}

TEST(PlayAudioModelTestCase, audioDeviceDescriptionsReturnsDescriptions) {
	const auto device = std::make_shared<AudioDeviceStub>();
	device->setDescriptions({ "a", "b", "c" });
	PlayAudioModelFacade model{ device };
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

class ReadsAOne : public AudioFrameReader {
	void read(float **audio, int) override {
		*audio[0] = 1;
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
};

class AudioTimesTwo : public AudioFrameProcessor {
	void process(float **audio, int) override {
		*audio[0] *= 2;
	}
};

TEST(PlayAudioModelTestCase, fillBufferReadsThenProcesses) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	PlayAudioModelFacade model{
		device,
		std::make_shared<AudioFrameProcessorStubFactory>(processor),
		std::make_shared<AudioFrameReaderStubFactory>(reader)
	};
	model.play();
	float x{};
	float *audio[] = { &x };
	device->fillStreamBuffer(audio, 1);
	EXPECT_EQ(2, x);
}
