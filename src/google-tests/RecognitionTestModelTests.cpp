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
	std::vector<int> preferredProcessingSizes_{};
	Initialization initialization_{};
	PlayRequest request_{};
	bool playing_{};
	bool playCalled_{};
public:
	void setPreferredProcessingSizes(std::vector<int> v) {
		preferredProcessingSizes_ = std::move(v);
	}

	std::vector<int> preferredProcessingSizes() override {
		return preferredProcessingSizes_;
	}

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
	StimulusPlayerStub player{};
	RecognitionTestModel model{ &list, &player };

	void initializeTest(Model::TestParameters p = {}) {
		model.initializeTest(std::move(p));
	}

	void playTrial(Model::TrialParameters p = {}) {
		model.playTrial(std::move(p));
	}
};

TEST_F(
    RecognitionTestModelTests,
    initializeTestInitializesStimulusList
) {
    Model::TestParameters p;
    p.audioDirectory = "a";
    initializeTest(p);
    assertEqual("a", list.directory());
}

TEST_F(RecognitionTestModelTests, initializeTestInitializesPlayer) {
	RecognitionTestModel::TestParameters p;
	p.leftDslPrescriptionFilePath = "a";
	p.rightDslPrescriptionFilePath = "b";
	p.brirFilePath = "c";
	p.attack_ms = 1;
	p.release_ms = 2;
	p.windowSize = 3;
	p.chunkSize = 4;
	initializeTest(p);
	assertEqual("a", player.initialization().leftDslPrescriptionFilePath);
	assertEqual("b", player.initialization().rightDslPrescriptionFilePath);
	assertEqual("c", player.initialization().brirFilePath);
	EXPECT_EQ(1, player.initialization().attack_ms);
	EXPECT_EQ(2, player.initialization().release_ms);
	EXPECT_EQ(3, player.initialization().windowSize);
	EXPECT_EQ(4, player.initialization().chunkSize);

	// The hearing aid simulation in MATLAB used 119 dB SPL as a maximum.
	// I don't think it's crucial for chapro, but I'll leave it as it was.
	EXPECT_EQ(119, player.initialization().max_dB_Spl);
}

TEST_F(
    RecognitionTestModelTests,
    playTrialPassesNextStimulusToStimulusPlayer
) {
    list.setNext("a");
	playTrial();
    assertEqual("a", player.request().audioFilePath);
}

TEST_F(RecognitionTestModelTests, playTrialPassesRequestToPlayer) {
	RecognitionTestModel::TrialParameters p;
	p.audioDevice = "a";
	p.level_dB_Spl = 1;
	playTrial(p);
	EXPECT_EQ(1, player.request().level_dB_Spl);
}

TEST_F(RecognitionTestModelTests, playTrialDoesNotAdvanceListWhenPlayerPlaying) {
	player.setPlaying();
	playTrial();
	EXPECT_FALSE(list.nextCalled());
}

TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayAgainWhenPlayerAlreadyPlaying) {
	player.setPlaying();
	playTrial();
	EXPECT_FALSE(player.playCalled());
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
	player.setAudioDeviceDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

TEST_F(RecognitionTestModelTests, preferredProcessingSizesReturnsThatOfTheAudioPlayer) {
	player.setPreferredProcessingSizes({ 1, 2, 3 });
	assertEqual({ 1, 2, 3 }, model.preferredProcessingSizes());
}

TEST_F(RecognitionTestModelTests, todoTest) {
	assertEqual({ 64, 128, 256, 512, 1024, 2048, 4096, 8192 }, model.preferredProcessingSizes());
}

class RequestFailingStimulusPlayer : public StimulusPlayer {
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
	std::vector<int> preferredProcessingSizes() override {
		return {};
	}
};

class RecognitionTestModelWithRequestFailingStimulusPlayer : public ::testing::Test {
protected:
	StimulusListStub list{};
	RequestFailingStimulusPlayer player{};
	RecognitionTestModel model{ &list, &player };

	void assertPlayTrialThrowsTrialFailure(std::string what) {
		try {
			model.playTrial({});
			FAIL() << "Expected RecognitionTestModel::TrialFailure";
		}
		catch (const RecognitionTestModel::TrialFailure &e) {
			assertEqual(std::move(what), e.what());
		}
	}
};

TEST_F(
	RecognitionTestModelWithRequestFailingStimulusPlayer,
	playTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
) {
	player.setErrorMessage("error.");
	assertPlayTrialThrowsTrialFailure("error.");
}

class InitializationFailingStimulusPlayer : public StimulusPlayer {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}
	void initialize(Initialization) override {
		throw InitializationFailure{ errorMessage };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	bool isPlaying() override { return {}; }
	void play(PlayRequest) override {}
	std::vector<int> preferredProcessingSizes() override {
		return {};
	}
};

class RecognitionTestModelWithInitializationFailingStimulusPlayer : public ::testing::Test {
protected:
	StimulusListStub list{};
	InitializationFailingStimulusPlayer player{};
	RecognitionTestModel model{ &list, &player };

	void assertInitializeTestThrowsInitializationFailure(std::string what) {
		try {
			model.initializeTest({});
			FAIL() << "Expected RecognitionTestModel::TestInitializationFailure";
		}
		catch (const RecognitionTestModel::TestInitializationFailure &e) {
			assertEqual(std::move(what), e.what());
		}
	}
};

TEST_F(
	RecognitionTestModelWithInitializationFailingStimulusPlayer,
	initializeTestThrowsTestInitializationFailureWhenPlayerThrowsInitializationFailure
) {
	player.setErrorMessage("error.");
	assertInitializeTestThrowsInitializationFailure("error.");
}
