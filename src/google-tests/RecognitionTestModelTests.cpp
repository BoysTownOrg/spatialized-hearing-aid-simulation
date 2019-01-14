#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.h>
#include <gtest/gtest.h>

class StimulusListStub : public StimulusList {
	std::string directory_{};
	std::string next_{};
	bool empty_{};
	bool nextCalled_{};
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
		nextCalled_ = true;
		return next_;
	}

	void setEmpty() {
		empty_ = true;
	}

	bool empty() override {
		return empty_;
	}

	bool nextCalled() const {
		return nextCalled_;
	}
};

class StimulusPlayerStub : public StimulusPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	PlayRequest request_{};
	bool playing_{};
public:
	const PlayRequest &request() const {
		return request_;
	}

	void play(PlayRequest request) override {
		request_ = std::move(request);
	}

	void setAudioDeviceDescriptions(std::vector<std::string> v) {
		audioDeviceDescriptions_ = std::move(v);
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
	}

	void setPlaying() {
		playing_ = true;
	}
	
	bool isPlaying() override {
		return playing_;
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
    assertEqual("a", stimulusPlayer.request().audioFilePath);
}

TEST_F(
    RecognitionTestModelTests,
    testCompleteWhenListEmpty
) {
    list.setEmpty();
    EXPECT_TRUE(model.testComplete());
}

TEST_F(
    RecognitionTestModelTests,
    audioDeviceDescriptionsReturnsThatOfTheAudioPlayer
) {
	stimulusPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

TEST_F(RecognitionTestModelTests, playTrialPassesParametersToPlayer) {
	RecognitionTestModel::TestParameters test;
	test.leftDslPrescriptionFilePath = "a";
	test.rightDslPrescriptionFilePath = "b";
	test.brirFilePath = "c";
	test.attack_ms = 1;
	test.release_ms = 2;
	test.windowSize = 3;
	test.chunkSize = 4;
	model.initializeTest(test);
	RecognitionTestModel::TrialParameters trial;
	trial.audioDevice = "d";
	trial.level_dB_Spl = 5;
	model.playTrial(trial);
	assertEqual("a", stimulusPlayer.request().leftDslPrescriptionFilePath);
	assertEqual("b", stimulusPlayer.request().rightDslPrescriptionFilePath);
	assertEqual("c", stimulusPlayer.request().brirFilePath);
	assertEqual("d", stimulusPlayer.request().audioDevice);
	EXPECT_EQ(1, stimulusPlayer.request().attack_ms);
	EXPECT_EQ(2, stimulusPlayer.request().release_ms);
	EXPECT_EQ(3, stimulusPlayer.request().windowSize);
	EXPECT_EQ(4, stimulusPlayer.request().chunkSize);
	EXPECT_EQ(5, stimulusPlayer.request().level_dB_Spl);
}

TEST_F(RecognitionTestModelTests, playTrialDoesNotAdvanceListWhenPlayerPlaying) {
	stimulusPlayer.setPlaying();
	model.playTrial({});
	EXPECT_FALSE(list.nextCalled());
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

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	bool isPlaying() override { return {}; }
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
	catch (const RecognitionTestModel::TrialFailure &e) {
		assertEqual("error.", e.what());
	}
}
