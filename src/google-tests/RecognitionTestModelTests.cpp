#include "assert-utility.h"
#include "StimulusPlayerStub.h"
#include "StimulusListStub.h"
#include "DocumenterStub.h"
#include <recognition-test/RecognitionTestModel.h>
#include <gtest/gtest.h>

namespace {
	class RecognitionTestModelTests : public ::testing::Test {
	protected:
		RecognitionTestModel::TestParameters testParameters;
		StimulusListStub list{};
		StimulusPlayerStub player{};
		DocumenterStub documenter{};
		RecognitionTestModel model{ &list, &player, &documenter };

		void initializeTest() {
			model.initializeTest(testParameters);
		}

		void playTrial(Model::TrialParameters p = {}) {
			model.playTrial(std::move(p));
		}
	};

	TEST_F(
		RecognitionTestModelTests,
		initializeTestInitializesStimulusList
	) {
		testParameters.audioDirectory = "a";
		initializeTest();
		assertEqual("a", list.directory());
	}

	TEST_F(RecognitionTestModelTests, initializeTestInitializesPlayer) {
		testParameters.leftDslPrescriptionFilePath = "a";
		testParameters.rightDslPrescriptionFilePath = "b";
		testParameters.brirFilePath = "c";
		testParameters.attack_ms = 1;
		testParameters.release_ms = 2;
		testParameters.windowSize = 3;
		testParameters.chunkSize = 4;
		initializeTest();
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
		initializeTestInitializesDocumenter
	) {
		testParameters.testFilePath = "a";
		initializeTest();
		assertEqual("a", documenter.filePath());
	}

	TEST_F(
		RecognitionTestModelTests,
		initializeTestDocumentsTestParameters
	) {
		testParameters.subjectId = "a";
		testParameters.testerId = "b";
		testParameters.leftDslPrescriptionFilePath = "c";
		testParameters.rightDslPrescriptionFilePath = "d";
		testParameters.brirFilePath = "e";
		testParameters.attack_ms = 1;
		testParameters.release_ms = 2;
		testParameters.windowSize = 3;
		testParameters.chunkSize = 4;
		initializeTest();
		assertEqual(
			"subject: a\n"
			"tester: b\n"
			"DSL prescription\n"
			"    left: c\n"
			"    right: d\n"
			"BRIR: e\n"
			"attack (ms): 1.0\n"
			"release (ms): 2.0\n"
			"window size (samples): 3\n"
			"chunk size (samples): 4\n\n", 
			documenter.content()
		);
	}

	TEST_F(
		RecognitionTestModelTests,
		initializeTestDocumentsTestParametersAfterInitializing
	) {
		initializeTest();
		EXPECT_TRUE(documenter.log().beginsWith("initialize"));
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
		RecognitionTestModel::TrialParameters trial;
		trial.audioDevice = "a";
		trial.level_dB_Spl = 1;
		playTrial(trial);
		EXPECT_EQ(1, player.request().level_dB_Spl);
		assertEqual("a", player.request().audioDevice);
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

	class RecognitionTestModelWithRequestFailingStimulusPlayer : public ::testing::Test {
	protected:
		StimulusListStub list{};
		RequestFailingStimulusPlayer player{};
		DocumenterStub documenter{};
		RecognitionTestModel model{ &list, &player, &documenter };

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

	class RecognitionTestModelWithInitializationFailingStimulusPlayer : public ::testing::Test {
	protected:
		StimulusListStub list{};
		InitializationFailingStimulusPlayer player{};
		DocumenterStub documenter{};
		RecognitionTestModel model{ &list, &player, &documenter };

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
}