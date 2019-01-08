#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.h>
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
	StimulusListStub list{};
	StimulusPlayerStub stimulusPlayer{};
	RecognitionTestModel model{ &list, &stimulusPlayer };
};

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

class FailingStimulusPlayer : public StimulusPlayer {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void play(PlayRequest) override {
		throw RequestFailure{ errorMessage };
	}
};

TEST(
	RecognitionTestModelOtherTests,
	playTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
) {
	StimulusListStub list{};
	FailingStimulusPlayer stimulusPlayer{};
	stimulusPlayer.setErrorMessage("error.");
	RecognitionTestModel model{ &list, &stimulusPlayer };
	try {
		model.playTrial({});
		FAIL() << "Expected RecognitionTestModel::TrialFailure";
	}
	catch (const RecognitionTestModel::RequestFailure &e) {
		assertEqual("error.", e.what());
	}
}
