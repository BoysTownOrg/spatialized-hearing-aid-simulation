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
	std::vector<std::string> audioDeviceDescriptions_{};
	std::string filePath_{};
	PlayRequest request_{};
public:
	std::string filePath() const {
		return filePath_;
	}

	const PlayRequest &request() const {
		return request_;
	}

	void play(PlayRequest request) override {
		filePath_ = request.audioFilePath;
		request_ = std::move(request);
	}

	void setAudioDeviceDescriptions(std::vector<std::string> v) {
		audioDeviceDescriptions_ = std::move(v);
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
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

TEST_F(
    RecognitionTestModelTests,
    audioDeviceDescriptionsReturnsThatOfTheAudioPlayer
) {
	stimulusPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

TEST_F(RecognitionTestModelTests, playTrialPassesParametersToPlayer) {
	RecognitionTestModel::TestParameters p;
	p.leftDslPrescriptionFilePath = "a";
	p.rightDslPrescriptionFilePath = "b";
	p.brirFilePath = "d";
	p.attack_ms = 2;
	p.release_ms = 3;
	p.windowSize = 4;
	p.chunkSize = 5;
	model.initializeTest(p);
	RecognitionTestModel::TrialParameters trial;
	trial.audioDevice = "gamma";
	trial.level_dB_Spl = 1;
	trial.audioDevice = "gamma";
	model.playTrial(trial);
	assertEqual("a", stimulusPlayer.request().leftDslPrescriptionFilePath);
	assertEqual("b", stimulusPlayer.request().rightDslPrescriptionFilePath);
	assertEqual("d", stimulusPlayer.request().brirFilePath);
	assertEqual("gamma", stimulusPlayer.request().audioDevice);
	EXPECT_EQ(1, stimulusPlayer.request().level_dB_Spl);
	EXPECT_EQ(2, stimulusPlayer.request().attack_ms);
	EXPECT_EQ(3, stimulusPlayer.request().release_ms);
	EXPECT_EQ(4, stimulusPlayer.request().windowSize);
	EXPECT_EQ(5, stimulusPlayer.request().chunkSize);
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
