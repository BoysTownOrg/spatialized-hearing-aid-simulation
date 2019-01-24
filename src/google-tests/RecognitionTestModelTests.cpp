#include "assert-utility.h"
#include "StimulusPlayerStub.h"
#include "FakeStimulusList.h"
#include "DocumenterStub.h"
#include <recognition-test/RecognitionTestModel.h>
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
	class RecognitionTestModelTests : public ::testing::Test {
	protected:
		RecognitionTestModel::TestParameters testParameters;
		RecognitionTestModel::TrialParameters trial;
		FakeStimulusList list{};
		StimulusPlayerStub player{};
		DocumenterStub documenter{};
		RecognitionTestModel model{ &list, &player, &documenter };

		void initializeTest() {
			model.initializeTest(testParameters);
		}

		void playTrial() {
			model.playTrial(trial);
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
		GlobalTestParameters global;
		testParameters.global = &global;
		initializeTest();
		EXPECT_EQ(&global, player.initialization().global);
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
		GlobalTestParameters global;
		testParameters.global = &global;
		initializeTest();
		EXPECT_EQ(&global, documenter.documentedTestParameters().global);
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
		list.setContents({ "a", "b", "c" });
		playTrial();
		assertEqual("a", player.request().audioFilePath);
		playTrial();
		assertEqual("b", player.request().audioFilePath);
		playTrial();
		assertEqual("c", player.request().audioFilePath);
	}

	TEST_F(RecognitionTestModelTests, playTrialPassesRequestToPlayer) {
		trial.audioDevice = "a";
		trial.level_dB_Spl = 1;
		playTrial();
		EXPECT_EQ(1, player.request().level_dB_Spl);
		assertEqual("a", player.request().audioDevice);
	}

	TEST_F(RecognitionTestModelTests, playTrialDoesNotAdvanceListWhenPlayerPlaying) {
		list.setContents({ "a", "b", "c" });
		playTrial();
		player.setPlaying();
		playTrial();
		assertEqual("a", player.request().audioFilePath);
	}

	TEST_F(RecognitionTestModelTests, playTrialDoesNotAdvanceListWhenPlayerFails) {
		list.setContents({ "a", "b", "c" });
		player.failOnPlay();
		try {
			playTrial();
		}
		catch (const RecognitionTestModel::TrialFailure &) {

		}
		player.dontFailOnPlay();
		playTrial();
		assertEqual("a", player.request().audioFilePath);
	}

	TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayAgainWhenPlayerAlreadyPlaying) {
		player.setPlaying();
		playTrial();
		EXPECT_FALSE(player.playCalled());
	}

	TEST_F(
		RecognitionTestModelTests,
		playTrialDocumentsTrial
	) {
		list.setContents({ "a", "b", "c" });
		trial.level_dB_Spl = 1;
		playTrial();
		EXPECT_EQ(1, documenter.globalTrialParameters.level_dB_Spl);
		assertEqual("a", documenter.globalTrialParameters.stimulus);
	}

	TEST_F(
		RecognitionTestModelTests,
		testCompleteWhenListEmpty
	) {
		list.setContents({});
		EXPECT_TRUE(model.testComplete());
	}

	TEST_F(
		RecognitionTestModelTests,
		audioDeviceDescriptionsReturnsThatOfTheAudioPlayer
	) {
		player.setAudioDeviceDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
	}

	class RecognitionTestModelWithInitializationFailingDocumenter : public ::testing::Test {
	protected:
		FakeStimulusList list{};
		StimulusPlayerStub player{};
		InitializationFailingDocumenter documenter{};
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
		RecognitionTestModelWithInitializationFailingDocumenter,
		initializeTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
	) {
		documenter.setErrorMessage("error.");
		assertInitializeTestThrowsInitializationFailure("error.");
	}

	class RecognitionTestModelWithInitializationFailingStimulusPlayer : public ::testing::Test {
	protected:
		FakeStimulusList list{};
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

	class RecognitionTestModelWithRequestFailingStimulusPlayer : public ::testing::Test {
	protected:
		FakeStimulusList list{};
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
}