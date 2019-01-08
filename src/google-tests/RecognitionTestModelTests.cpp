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
	AudioFrameReaderStubFactory readerFactory{};
	AudioFrameProcessorStubFactory processorFactory{};
	StimulusListStub list{};
	StimulusPlayerStub stimulusPlayer{};
	RecognitionTestModel model{ &device, &readerFactory, &processorFactory, &list, &stimulusPlayer };
};

TEST_F(
	RecognitionTestModelTests,
	DISABLED_playTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
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
