#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "AudioDeviceStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <playing-audio/RecognitionTestModel.h>
#include <gtest/gtest.h>

class RecognitionTestModelFacade {
	RecognitionTestModel model;
public:
	RecognitionTestModelFacade(std::shared_ptr<AudioDevice> device) :
		model{
			std::move(device),
			std::make_shared<AudioFrameReaderStubFactory>(),
			std::make_shared<AudioFrameProcessorStubFactory>()
		}
	{}
};

class RecognitionTestModelTests : public ::testing::Test {
protected:
	std::shared_ptr<AudioDeviceStub> device = std::make_shared<AudioDeviceStub>();
	std::shared_ptr<AudioFrameReaderStub> frameReader = std::make_shared<AudioFrameReaderStub>();
	std::shared_ptr<AudioFrameReaderStubFactory> readerFactory =
		std::make_shared<AudioFrameReaderStubFactory>(frameReader);
	std::shared_ptr<AudioFrameProcessorStub> processor = std::make_shared<AudioFrameProcessorStub>();
	std::shared_ptr<AudioFrameProcessorStubFactory> processorFactory =
		std::make_shared<AudioFrameProcessorStubFactory>(processor);
	RecognitionTestModel model{ device, readerFactory, processorFactory };
	
	void assertInitializeTestThrowsInitializationFailure(
		std::string errorMessage
	) {
		try {
			model.initializeTest({});
			FAIL() << "Expected RecognitionTestModel::TestInitializationFailure";
		}
		catch (const RecognitionTestModel::TestInitializationFailure &e) {
			assertEqual(errorMessage, e.what());
		}
	}

	void assertPlayTrialThrowsRequestFailure(std::string errorMessage) {
		try {
			model.playTrial({});
			FAIL() << "Expected RecognitionTestModel::RequestFailure";
		}
		catch (const RecognitionTestModel::RequestFailure &e) {
			assertEqual(errorMessage, e.what());
		}
	}
};

TEST_F(RecognitionTestModelTests, constructorSetsItself) {
	EXPECT_EQ(&model, device->controller());
}

TEST(
	PlayAudioModelDeviceFailureTests,
	constructorThrowsDeviceFailureWhenDeviceFailsToInitialize
) {
	const auto device = std::make_shared<AudioDeviceStub>();
	device->fail();
	device->setErrorMessage("error.");
	try {
		RecognitionTestModelFacade model{ device };
		FAIL() << "Expected RecognitionTestModel::DeviceFailure";
	}
	catch (const RecognitionTestModel::DeviceFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST_F(RecognitionTestModelTests, playTrialFirstClosesStreamThenOpensThenStarts) {
	model.playTrial({});
	assertEqual("close open start ", device->streamLog());
}

TEST_F(
	RecognitionTestModelTests,
	playTrialThrowsRequestFailureWhenDeviceFails
) {
	device->fail();
	device->setErrorMessage("error.");
	assertPlayTrialThrowsRequestFailure("error.");
}

TEST(
	PlayAudioModelErroredFrameReaderFactoryTest,
	initializeTestThrowsInitializationFailureWhenReaderFactoryThrowsCreateError
) {
	RecognitionTestModel model{ 
		std::make_shared<AudioDeviceStub>(), 
		std::make_shared<ErrorAudioFrameReaderFactory>("error."), 
		std::make_shared<AudioFrameProcessorStubFactory>() 
	};
	try {
		model.initializeTest({});
		FAIL() << "Expected RecognitionTestModel::TestInitializationFailure";
	}
	catch (const RecognitionTestModel::TestInitializationFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(
	PlayAudioModelErroredProcessorFactoryTest,
	initializeTestThrowsInitializationFailureWhenProcessorFactoryThrowsCreateError)
{
	RecognitionTestModel model{ 
		std::make_shared<AudioDeviceStub>(), 
		std::make_shared<AudioFrameReaderStubFactory>(), 
		std::make_shared<ErrorAudioFrameProcessorFactory>("error.") 
	};
	try {
		model.initializeTest({});
		FAIL() << "Expected RecognitionTestModel::TestInitializationFailure";
	}
	catch (const RecognitionTestModel::TestInitializationFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST_F(RecognitionTestModelTests, playTrialWhileStreamingDoesNotAlterCurrentStream) {
	device->setStreaming();
	model.playTrial({});
	EXPECT_TRUE(device->streamLog().empty());
}

TEST_F(RecognitionTestModelTests, playTrialPassesParametersToFactories) {
	RecognitionTestModel::PlayRequest request;
	device->setDescriptions({ "alpha", "beta", "gamma", "lambda" });
	request.audioDevice = "gamma";
	frameReader->setChannels(6);
	frameReader->setSampleRate(7);
	model.playTrial(request);
	EXPECT_EQ(2, device->streamParameters().deviceIndex);
	EXPECT_EQ(6, device->streamParameters().channels);
	EXPECT_EQ(7, device->streamParameters().sampleRate);
}

TEST_F(RecognitionTestModelTests, initializeTestPassesParametersToProcessorFactory) {
	Model::TestParameters p;
	p.leftDslPrescriptionFilePath = "a";
	p.rightDslPrescriptionFilePath = "b";
	p.brirFilePath = "c";
	p.level_dB_Spl = 1;
	p.attack_ms = 2;
	p.release_ms = 3;
	p.windowSize = 4;
	p.chunkSize = 5;
	model.initializeTest(p);
	assertEqual("a", processorFactory->parameters().leftDslPrescriptionFilePath);
	assertEqual("b", processorFactory->parameters().rightDslPrescriptionFilePath);
	assertEqual("c", processorFactory->parameters().brirFilePath);
	EXPECT_EQ(1, processorFactory->parameters().level_dB_Spl);
	EXPECT_EQ(2, processorFactory->parameters().attack_ms);
	EXPECT_EQ(3, processorFactory->parameters().release_ms);
	EXPECT_EQ(4, processorFactory->parameters().windowSize);
	EXPECT_EQ(5, processorFactory->parameters().chunkSize);
}

TEST_F(RecognitionTestModelTests, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	model.initializeTest({});
	frameReader->setChannels(0);
	model.playTrial({});
	device->fillStreamBuffer(nullptr, 0);
	EXPECT_FALSE(device->setCallbackResultToCompleteCalled());
	frameReader->setComplete();
	device->fillStreamBuffer(nullptr, 0);
	EXPECT_TRUE(device->setCallbackResultToCompleteCalled());
}

TEST_F(RecognitionTestModelTests, fillStreamBufferPassesAudio) {
	model.initializeTest({});
	frameReader->setChannels(2);
	model.playTrial({});
	float left{};
	float right{};
	float *x[]{ &left, &right };
	device->fillStreamBuffer(x, 1);
	EXPECT_EQ(&left, frameReader->audioBuffer()[0].data());
	EXPECT_EQ(&right, frameReader->audioBuffer()[1].data());
	EXPECT_EQ(1, frameReader->audioBuffer()[0].size());
	EXPECT_EQ(1, frameReader->audioBuffer()[1].size());
	EXPECT_EQ(&left, processor->audioBuffer()[0].data());
	EXPECT_EQ(&right, processor->audioBuffer()[1].data());
	EXPECT_EQ(1, processor->audioBuffer()[0].size());
	EXPECT_EQ(1, processor->audioBuffer()[1].size());
}

TEST_F(RecognitionTestModelTests, playTrialSetsCallbackResultToContinueBeforeStartingStream) {
	model.playTrial({});
	assertEqual("setCallbackResultToContinue start ", device->callbackLog());
}

TEST_F(RecognitionTestModelTests, audioDeviceDescriptionsReturnsDescriptions) {
	device->setDescriptions({ "a", "b", "c" });
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

TEST(PlayAudioModelProcessOrderTest, fillBufferReadsThenProcesses) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	RecognitionTestModel model{
		device,
		std::make_shared<AudioFrameReaderStubFactory>(reader),
		std::make_shared<AudioFrameProcessorStubFactory>(processor)
	};
	model.initializeTest({});
	model.playTrial({});
	float x{};
	float *audio[] = { &x };
	device->fillStreamBuffer(audio, 1);
	EXPECT_EQ(2, x);
}

TEST(PlayAudioModelRmsTest, playPassesComputedRmsToProcessorFactory) {
	FakeAudioFileReader reader{
		std::vector<float>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		2
	};
	const auto processorFactory = std::make_shared<AudioFrameProcessorStubFactory>();
	RecognitionTestModel model{
		std::make_shared<AudioDeviceStub>(),
		std::make_shared<AudioFrameReaderStubFactory>(
			std::make_shared<AudioFileInMemory>(reader)),
		processorFactory
	};
	model.play({});
	assertEqual(
		{ 
			std::sqrt((1*1 + 3*3 + 5*5 + 7*7 + 9*9) / 5),
			std::sqrt((2*2 + 4*4 + 6*6 + 8*8 + 10*10) / 5)
		}, 
		processorFactory->parameters().stimulusRms,
		1e-6);
}

TEST_F(RecognitionTestModelTests, playResetsReaderAfterComputingRms) {
	model.play({});
	EXPECT_TRUE(frameReader->readingLog().endsWith("reset "));
}
