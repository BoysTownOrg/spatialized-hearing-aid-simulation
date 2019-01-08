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
