#include "assert-utility.h"
#include "StimulusPlayerStub.h"
#include "FakeStimulusList.h"
#include "DocumenterStub.h"
#include <recognition-test/RecognitionTest.h>
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
	class RecognitionTestTests : public ::testing::Test {
	protected:
		RecognitionTest::TestParameters testParameters;
		RecognitionTest::TrialParameters trial;
		RecognitionTest::CalibrationParameters calibration;
		FakeStimulusList list{};
		StimulusPlayerStub player{};
		DocumenterStub documenter{};
		RecognitionTest model{ &list, &player, &documenter };

		void initializeTest() {
			model.initializeTest(testParameters);
		}

		void playTrial() {
			model.playTrial(trial);
		}

		void playCalibration() {
			model.playCalibration(calibration);
		}
	};

	TEST_F(
		RecognitionTestTests,
		initializeTestInitializesStimulusList
	) {
		testParameters.audioDirectory = "a";
		initializeTest();
		assertEqual("a", list.directory());
	}

	TEST_F(
		RecognitionTestTests,
		initializeTestInitializesDocumenter
	) {
		testParameters.testFilePath = "a";
		initializeTest();
		assertEqual("a", documenter.filePath());
	}

	TEST_F(
		RecognitionTestTests,
		initializeTestDocumentsTestParameters
	) {
		GlobalTestParameters global;
		testParameters.global = &global;
		initializeTest();
		EXPECT_EQ(&global, documenter.documentedTestParameters().global);
	}

	TEST_F(
		RecognitionTestTests,
		initializeTestDocumentsTestParametersAfterInitializing
	) {
		initializeTest();
		EXPECT_TRUE(documenter.log().beginsWith("initialize"));
	}

	TEST_F(
		RecognitionTestTests,
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

	TEST_F(RecognitionTestTests, playTrialPassesRequestToPlayer) {
		trial.audioDevice = "a";
		trial.level_dB_Spl = 1;
		playTrial();
		EXPECT_EQ(1, player.request().level_dB_Spl);
		assertEqual("a", player.request().audioDevice);
	}

	TEST_F(RecognitionTestTests, playTrialDoesNotAdvanceListWhenPlayerPlaying) {
		list.setContents({ "a", "b", "c" });
		playTrial();
		player.setPlaying();
		playTrial();
		assertEqual("a", player.request().audioFilePath);
	}

	TEST_F(RecognitionTestTests, playTrialDoesNotAdvanceListWhenPlayerFails) {
		list.setContents({ "a", "b", "c" });
		player.failOnPlay();
		try {
			playTrial();
		}
		catch (const RecognitionTest::TrialFailure &) {

		}
		player.dontFailOnPlay();
		playTrial();
		assertEqual("a", player.request().audioFilePath);
	}

	TEST_F(RecognitionTestTests, playTrialDoesNotPlayAgainWhenPlayerAlreadyPlaying) {
		player.setPlaying();
		playTrial();
		EXPECT_FALSE(player.playCalled());
	}

	TEST_F(
		RecognitionTestTests,
		playTrialDocumentsTrial
	) {
		list.setContents({ "a", "b", "c" });
		trial.level_dB_Spl = 1;
		playTrial();
		EXPECT_EQ(1, documenter.globalTrialParameters.level_dB_Spl);
		assertEqual("a", documenter.globalTrialParameters.stimulus);
	}

	TEST_F(RecognitionTestTests, playCalibrationPassesRequestToPlayer) {
		calibration.audioDevice = "a";
		calibration.audioFilePath = "b";
		calibration.level_dB_Spl = 1;
		playCalibration();
		assertEqual("a", player.request().audioDevice);
		assertEqual("b", player.request().audioFilePath);
		EXPECT_EQ(1, player.request().level_dB_Spl);
	}

	TEST_F(RecognitionTestTests, stopCalibrationStopsPlayer) {
		model.stopCalibration();
		EXPECT_TRUE(player.stopped());
	}

	TEST_F(
		RecognitionTestTests,
		testCompleteWhenListEmpty
	) {
		list.setContents({});
		EXPECT_TRUE(model.testComplete());
	}

	TEST_F(
		RecognitionTestTests,
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
		RecognitionTest model{ &list, &player, &documenter };

		void assertInitializeTestThrowsInitializationFailure(std::string what) {
			try {
				model.initializeTest({});
				FAIL() << "Expected RecognitionTest::TestInitializationFailure";
			}
			catch (const RecognitionTest::TestInitializationFailure &e) {
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

	class RecognitionTestModelWithRequestFailingStimulusPlayer : public ::testing::Test {
	protected:
		FakeStimulusList list{};
		RequestFailingStimulusPlayer player{};
		DocumenterStub documenter{};
		RecognitionTest model{ &list, &player, &documenter };

		void assertPlayTrialThrowsTrialFailure(std::string what) {
			try {
				model.playTrial({});
				FAIL() << "Expected RecognitionTest::TrialFailure";
			}
			catch (const RecognitionTest::TrialFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayCalibrationThrowsCalibrationFailure(std::string what) {
			try {
				model.playCalibration({});
				FAIL() << "Expected RecognitionTest::CalibrationFailure";
			}
			catch (const RecognitionTest::CalibrationFailure &e) {
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

	TEST_F(
		RecognitionTestModelWithRequestFailingStimulusPlayer,
		playCalibrationThrowsCalibrationFailureWhenPlayerThrowsRequestFailure
	) {
		player.setErrorMessage("error.");
		assertPlayCalibrationThrowsCalibrationFailure("error.");
	}
}