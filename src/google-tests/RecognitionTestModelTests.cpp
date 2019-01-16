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
		directory_ = std::move(d);
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
	Initialization initialization_{};
	PlayRequest request_{};
	bool playing_{};
	bool playCalled_{};
public:
	const Initialization &initialization() const {
		return initialization_;
	}

	const PlayRequest &request() const {
		return request_;
	}

	void play(PlayRequest request) override {
		request_ = std::move(request);
		playCalled_ = true;
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

	bool playCalled() {
		return playCalled_;
	}

	void initialize(Initialization i) override {
		initialization_ = std::move(i);
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

TEST_F(RecognitionTestModelTests, initializeTestPassesParametersToPlayer) {
	RecognitionTestModel::TestParameters test;
	test.leftDslPrescriptionFilePath = "a";
	test.rightDslPrescriptionFilePath = "b";
	test.brirFilePath = "c";
	test.attack_ms = 1;
	test.release_ms = 2;
	test.windowSize = 3;
	test.chunkSize = 4;
	model.initializeTest(test);
	assertEqual("a", stimulusPlayer.initialization().leftDslPrescriptionFilePath);
	assertEqual("b", stimulusPlayer.initialization().rightDslPrescriptionFilePath);
	assertEqual("c", stimulusPlayer.initialization().brirFilePath);
	EXPECT_EQ(1, stimulusPlayer.initialization().attack_ms);
	EXPECT_EQ(2, stimulusPlayer.initialization().release_ms);
	EXPECT_EQ(3, stimulusPlayer.initialization().windowSize);
	EXPECT_EQ(4, stimulusPlayer.initialization().chunkSize);

	// The hearing aid simulation in MATLAB used 119 dB SPL as a maximum.
	// I don't think it's crucial for chapro, but I'll leave it as it was.
	EXPECT_EQ(119, stimulusPlayer.initialization().max_dB_Spl);
}

TEST_F(RecognitionTestModelTests, playTrialPassesParametersToPlayer) {
	RecognitionTestModel::TrialParameters trial;
	trial.audioDevice = "a";
	trial.level_dB_Spl = 1;
	model.playTrial(trial);
	EXPECT_EQ(1, stimulusPlayer.request().level_dB_Spl);
}

TEST_F(RecognitionTestModelTests, playTrialDoesNotAdvanceListWhenPlayerPlaying) {
	stimulusPlayer.setPlaying();
	model.playTrial({});
	EXPECT_FALSE(list.nextCalled());
}

TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayAgainWhenPlayerPlaying) {
	stimulusPlayer.setPlaying();
	model.playTrial({});
	EXPECT_FALSE(stimulusPlayer.playCalled());
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
	void initialize(Initialization) override {}
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
