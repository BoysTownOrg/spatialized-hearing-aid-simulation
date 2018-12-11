#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "AudioDeviceStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <playing-audio/PlayAudioModel.h>
#include <gtest/gtest.h>

class PlayAudioModelFacade {
	PlayAudioModel model;
public:
	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioDevice> device =
			std::make_shared<AudioDeviceStub>(),
		std::shared_ptr<AudioFrameReaderFactory> audioFactory =
			std::make_shared<AudioFrameReaderStubFactory>(),
		std::shared_ptr<AudioFrameProcessorFactory> processorFactory =
			std::make_shared<AudioFrameProcessorStubFactory>()
	) :
		model{ 
			std::move(device),
			std::move(audioFactory),
			std::move(processorFactory)
		} {}

	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioFrameReaderFactory> audioFactory =
			std::make_shared<AudioFrameReaderStubFactory>(),
		std::shared_ptr<AudioFrameProcessorFactory> processorFactory =
			std::make_shared<AudioFrameProcessorStubFactory>()
	) :
		PlayAudioModelFacade{
			std::make_shared<AudioDeviceStub>(),
			std::move(audioFactory),
			std::move(processorFactory)
		} {}

	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioFrameProcessorFactory> processorFactory =
			std::make_shared<AudioFrameProcessorStubFactory>()
	) :
		PlayAudioModelFacade{
			std::make_shared<AudioFrameReaderStubFactory>(),
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

static void assertPlayThrowsRequestError(
	PlayAudioModelFacade &model, 
	std::string errorMessage
) {
	try {
		model.play();
		FAIL() << "Expected PlayAudioModel::RequestFailure";
	}
	catch (const PlayAudioModel::RequestFailure &e) {
		assertEqual(errorMessage, e.what());
	}
}

TEST(
	PlayAudioModelTestCase,
	playThrowsRequestErrorWhenDeviceFailure)
{
	const auto device = std::make_shared<AudioDeviceStub>();
	PlayAudioModelFacade model{ device };
	device->fail();
	device->setErrorMessage("error.");
	assertPlayThrowsRequestError(model, "error.");
}

TEST(
	PlayAudioModelTestCase,
	playThrowsRequestErrorWhenReaderFactoryThrowsCreateError)
{
	PlayAudioModelFacade model{ std::make_shared<ErrorAudioFrameReaderFactory>("error.") };
	assertPlayThrowsRequestError(model, "error.");
}

TEST(
	PlayAudioModelTestCase,
	playThrowsRequestErrorWhenProcessorFactoryThrowsCreateError)
{
	PlayAudioModelFacade model{ std::make_shared<ErrorAudioFrameProcessorFactory>("error.") };
	assertPlayThrowsRequestError(model, "error.");
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
		audioFactory,
		processorFactory
	};
	PlayAudioModel::PlayRequest request;
	request.leftDslPrescriptionFilePath = "a";
	request.rightDslPrescriptionFilePath = "b";
	request.audioFilePath = "c";
	request.brirFilePath = "d";
	device->setDescriptions({ "alpha", "beta", "gamma", "lambda" });
	request.audioDevice = "gamma";
	request.level_dB_Spl = 1;
	request.attack_ms = 2;
	request.release_ms = 3;
	request.windowSize = 4;
	request.chunkSize = 5;
	reader->setChannels(6);
	reader->setSampleRate(7);
	model.play(request);
	assertEqual("a", processorFactory->parameters().leftDslPrescriptionFilePath);
	assertEqual("b", processorFactory->parameters().rightDslPrescriptionFilePath);
	assertEqual("c", audioFactory->filePath());
	assertEqual("d", processorFactory->parameters().brirFilePath);
	EXPECT_EQ(2, device->streamParameters().deviceIndex);
	EXPECT_EQ(1, processorFactory->parameters().level_dB_Spl);
	EXPECT_EQ(2, processorFactory->parameters().attack_ms);
	EXPECT_EQ(3, processorFactory->parameters().release_ms);
	EXPECT_EQ(4, processorFactory->parameters().windowSize);
	EXPECT_EQ(5, processorFactory->parameters().chunkSize);
	EXPECT_EQ(5, device->streamParameters().framesPerBuffer);
	EXPECT_EQ(6, device->streamParameters().channels);
	EXPECT_EQ(7, device->streamParameters().sampleRate);
}

TEST(PlayAudioModelTestCase, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	reader->setChannels(0);
	PlayAudioModelFacade model{ device, std::make_shared<AudioFrameReaderStubFactory>(reader) };
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
	reader->setChannels(2);
	const auto processor = std::make_shared<AudioFrameProcessorStub>();
	PlayAudioModelFacade model{ 
		device,
		std::make_shared<AudioFrameReaderStubFactory>(reader),
		std::make_shared<AudioFrameProcessorStubFactory>(processor)
	};
	model.play();
	float left{};
	float right{};
	float *x[]{ &left, &right };
	device->fillStreamBuffer(x, 1);
	EXPECT_EQ(&left, reader->audioBuffer()[0].data());
	EXPECT_EQ(&right, reader->audioBuffer()[1].data());
	EXPECT_EQ(1, reader->audioBuffer()[0].size());
	EXPECT_EQ(1, reader->audioBuffer()[1].size());
	EXPECT_EQ(&left, processor->audioBuffer()[0].data());
	EXPECT_EQ(&right, processor->audioBuffer()[1].data());
	EXPECT_EQ(1, processor->audioBuffer()[0].size());
	EXPECT_EQ(1, processor->audioBuffer()[1].size());
}

TEST(PlayAudioModelTestCase, playSetsCallbackResultToContinueBeforeStartingStream) {
	const auto device = std::make_shared<AudioDeviceStub>();
	PlayAudioModelFacade model{ device };
	model.play();
	assertEqual("setCallbackResultToContinue start ", device->callbackLog());
}

TEST(PlayAudioModelTestCase, audioDeviceDescriptionsReturnsDescriptions) {
	const auto device = std::make_shared<AudioDeviceStub>();
	device->setDescriptions({ "a", "b", "c" });
	PlayAudioModelFacade model{ device };
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
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

TEST(PlayAudioModelTestCase, fillBufferReadsThenProcesses) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	PlayAudioModelFacade model{
		device,
		std::make_shared<AudioFrameReaderStubFactory>(reader),
		std::make_shared<AudioFrameProcessorStubFactory>(processor)
	};
	model.play();
	float x{};
	float *audio[] = { &x };
	device->fillStreamBuffer(audio, 1);
	EXPECT_EQ(2, x);
}

TEST(PlayAudioModelTestCase, playPassesComputedRmsToProcessorFactory) {
	FakeAudioFileReader reader{
		std::vector<float>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		2
	};
	const auto processorFactory = std::make_shared<AudioFrameProcessorStubFactory>();
	PlayAudioModelFacade model{
		std::make_shared<AudioFrameReaderStubFactory>(
			std::make_shared<AudioFileInMemory>(reader)),
		processorFactory
	};
	model.play();
	assertEqual(
		{ 
			std::sqrt((1*1 + 3*3 + 5*5 + 7*7 + 9*9) / 5),
			std::sqrt((2*2 + 4*4 + 6*6 + 8*8 + 10*10) / 5)
		}, 
		processorFactory->parameters().stimulusRms,
		1e-6);
}

TEST(PlayAudioModelTestCase, playResetsReaderAfterComputingRms) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	PlayAudioModelFacade model{
		std::make_shared<AudioFrameReaderStubFactory>(reader),
	};
	model.play();
	EXPECT_TRUE(reader->readingLog().endsWith("reset "));
}
