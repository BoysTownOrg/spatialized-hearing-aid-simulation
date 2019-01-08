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

	void play(PlayRequest request) override {
		filePath_ = request.audioFilePath;
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

TEST_F(
	RecognitionTestModelTests,
	DISABLED_playTrialThrowsRequestFailureWhenPlayerThrowsDeviceFailure
) {
	FAIL();
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
