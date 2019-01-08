#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "AudioDeviceStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <playing-audio/RecognitionTestModel.h>
#include <gtest/gtest.h>

class StimulusListStub : public StimulusList {
	std::string directory_{};
	std::string next_{};
	bool empty_{};
public:
	void setNext(std::string s) {
		next_ = std::move(s);
	}

	std::string directory() const {
		return directory_;
	}

	void initialize(std::string d) override {
		directory_ = d;
	}

	std::string next() override {
		return next_;
	}

	void setEmpty() {
		empty_ = true;
	}

	bool empty() override {
		return empty_;
	}
};

class StimulusPlayerStub : public StimulusPlayer {
	std::string filePath_{};
public:
	std::string filePath() const {
		return filePath_;
	}

	void play(std::string filePath) override {
		filePath_ = filePath;
	}
};

class RecognitionTestModelFacade {
	AudioDeviceStub device{};
	AudioFrameReaderStubFactory readerFactory{};
	AudioFrameProcessorStubFactory processorFactory{};
	StimulusListStub list{};
	StimulusPlayerStub player{};
	RecognitionTestModel model;
public:
	RecognitionTestModelFacade(AudioDevice *device) :
		model{
			device,
			&readerFactory,
			&processorFactory,
			&list,
			&player
		}
	{}

	RecognitionTestModelFacade(AudioFrameReaderFactory *readerFactory) :
		model{
			&device,
			readerFactory,
			&processorFactory,
			&list,
			&player
		}
	{}

	RecognitionTestModelFacade(AudioFrameProcessorFactory *processorFactory) :
		model{
			&device,
			&readerFactory,
			processorFactory,
			&list,
			&player
		}
	{}

	void initializeTest() {
		model.initializeTest({});
	}
};

class RecognitionTestModelTests : public ::testing::Test {
protected:
	AudioDeviceStub device{};
	std::shared_ptr<AudioFrameReaderStub> frameReader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory readerFactory{ frameReader };
	std::shared_ptr<AudioFrameProcessorStub> processor = std::make_shared<AudioFrameProcessorStub>();
	AudioFrameProcessorStubFactory processorFactory{ processor };
	StimulusListStub list{};
	StimulusPlayerStub stimulusPlayer{};
	RecognitionTestModel model{ &device, &readerFactory, &processorFactory, &list, &stimulusPlayer };
	
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
	EXPECT_EQ(&model, device.controller());
}

TEST_F(RecognitionTestModelTests, playTrialFirstClosesStreamThenOpensThenStarts) {
	model.playTrial({});
	assertEqual("close open start ", device.streamLog());
}

TEST_F(
	RecognitionTestModelTests,
	playTrialThrowsRequestFailureWhenDeviceFails
) {
	device.fail();
	device.setErrorMessage("error.");
	assertPlayTrialThrowsRequestFailure("error.");
}

TEST_F(RecognitionTestModelTests, playTrialWhileStreamingDoesNotAlterCurrentStream) {
	device.setStreaming();
	model.playTrial({});
	EXPECT_TRUE(device.streamLog().empty());
}

TEST_F(RecognitionTestModelTests, playTrialPassesParametersToFactories) {
	RecognitionTestModel::PlayRequest request;
	device.setDescriptions({ "alpha", "beta", "gamma", "lambda" });
	request.audioDevice = "gamma";
	frameReader->setChannels(6);
	frameReader->setSampleRate(7);
	model.playTrial(request);
	EXPECT_EQ(2, device.streamParameters().deviceIndex);
	EXPECT_EQ(6, device.streamParameters().channels);
	EXPECT_EQ(7, device.streamParameters().sampleRate);
}

TEST_F(
    RecognitionTestModelTests,
    initializeTestPassesStimulusListDirectoryToStimulusList
) {
    Model::TestParameters parameters;
    parameters.audioDirectory = "a";
    model.initializeTest(parameters);
    assertEqual("a", list.directory());
}

TEST_F(
    RecognitionTestModelTests,
    playTrialPassesNextStimulusToStimulusPlayer
) {
    list.setNext("a");
	model.playTrial({});
    assertEqual("a", stimulusPlayer.filePath());
}

TEST_F(
    RecognitionTestModelTests,
    testCompleteWhenListEmpty
) {
    list.setEmpty();
    EXPECT_TRUE(model.testComplete());
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
	assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
	assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
	assertEqual("c", processorFactory.parameters().brirFilePath);
	EXPECT_EQ(1, processorFactory.parameters().level_dB_Spl);
	EXPECT_EQ(2, processorFactory.parameters().attack_ms);
	EXPECT_EQ(3, processorFactory.parameters().release_ms);
	EXPECT_EQ(4, processorFactory.parameters().windowSize);
	EXPECT_EQ(5, processorFactory.parameters().chunkSize);
}

TEST_F(RecognitionTestModelTests, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	model.initializeTest({});
	frameReader->setChannels(0);
	model.playTrial({});
	device.fillStreamBuffer(nullptr, 0);
	EXPECT_FALSE(device.setCallbackResultToCompleteCalled());
	frameReader->setComplete();
	device.fillStreamBuffer(nullptr, 0);
	EXPECT_TRUE(device.setCallbackResultToCompleteCalled());
}

TEST_F(RecognitionTestModelTests, fillStreamBufferPassesAudio) {
	model.initializeTest({});
	frameReader->setChannels(2);
	model.playTrial({});
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

TEST_F(RecognitionTestModelTests, playTrialSetsCallbackResultToContinueBeforeStartingStream) {
	model.playTrial({});
	assertEqual("setCallbackResultToContinue start ", device.callbackLog());
}

TEST_F(RecognitionTestModelTests, audioDeviceDescriptionsReturnsDescriptions) {
	device.setDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

TEST_F(RecognitionTestModelTests, playResetsReaderAfterComputingRms) {
	model.play({});
	EXPECT_TRUE(frameReader->readingLog().endsWith("reset "));
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

TEST_F(RecognitionTestModelTests, fillBufferReadsThenProcesses) {
	readerFactory.setReader(std::make_shared<ReadsAOne>());
	processorFactory.setProcessor(std::make_shared<AudioTimesTwo>());
	model.initializeTest({});
	model.playTrial({});
	float x{};
	float *audio[] = { &x };
	device.fillStreamBuffer(audio, 1);
	EXPECT_EQ(2, x);
}

TEST_F(RecognitionTestModelTests, playPassesComputedRmsToProcessorFactory) {
	FakeAudioFileReader fake{
		std::vector<float>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		2
	};
	readerFactory.setReader(std::make_shared<AudioFileInMemory>(fake));
	model.play({});
	assertEqual(
		{ 
			std::sqrt((1*1 + 3*3 + 5*5 + 7*7 + 9*9) / 5),
			std::sqrt((2*2 + 4*4 + 6*6 + 8*8 + 10*10) / 5)
		}, 
		processorFactory.parameters().stimulusRms,
		1e-6
	);
}

TEST(
	RecognitionTestModelOtherTests,
	constructorThrowsDeviceFailureWhenDeviceFailsToInitialize
) {
	AudioDeviceStub device{};
	device.fail();
	device.setErrorMessage("error.");
	try {
		RecognitionTestModelFacade model{ &device };
		FAIL() << "Expected RecognitionTestModel::DeviceFailure";
	}
	catch (const RecognitionTestModel::DeviceFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(
	RecognitionTestModelOtherTests,
	initializeTestThrowsInitializationFailureWhenReaderFactoryThrowsCreateError
) {
	ErrorAudioFrameReaderFactory factory{ "error." };
	RecognitionTestModelFacade model{ &factory };
	try {
		model.initializeTest();
		FAIL() << "Expected RecognitionTestModel::TestInitializationFailure";
	}
	catch (const RecognitionTestModel::TestInitializationFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(
	RecognitionTestModelOtherTests,
	initializeTestThrowsInitializationFailureWhenProcessorFactoryThrowsCreateError)
{
	ErrorAudioFrameProcessorFactory factory{ "error." };
	RecognitionTestModelFacade model{ &factory };
	try {
		model.initializeTest();
		FAIL() << "Expected RecognitionTestModel::TestInitializationFailure";
	}
	catch (const RecognitionTestModel::TestInitializationFailure &e) {
		assertEqual("error.", e.what());
	}
}
