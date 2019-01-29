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
		RecognitionTest::TestParameters newTest;
		RecognitionTest::TrialParameters nextTrial;
		RecognitionTest::CalibrationParameters calibration;
		FakeStimulusList list{};
		StimulusPlayerStub player{};
		DocumenterStub documenter{};
		RecognitionTest model{ &list, &player, &documenter };

		void prepareNewTest() {
			model.prepareNewTest(newTest);
		}

		void prepareNextTrial() {
			model.prepareNextTrial(nextTrial);
		}

		void playCalibration() {
			model.playCalibration(calibration);
		}
	};

	TEST_F(
		RecognitionTestTests,
		prepareNewTestInitializesStimulusList
	) {
		newTest.audioDirectory = "a";
		prepareNewTest();
		assertEqual("a", list.directory());
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestInitializesDocumenter
	) {
		newTest.testFilePath = "a";
		prepareNewTest();
		assertEqual("a", documenter.filePath());
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestDocumentsTestParameters
	) {
		GlobalTestParameters global;
		newTest.global = &global;
		prepareNewTest();
		EXPECT_EQ(&global, documenter.documentedTestParameters().global);
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestDocumentsTestParametersAfterInitializing
	) {
		prepareNewTest();
		EXPECT_TRUE(documenter.log().beginsWith("initialize"));
	}

	TEST_F(
		RecognitionTestTests,
		prepareNextTrialPreparesNextStimulus
	) {
		list.setContents({ "a", "b", "c" });
		prepareNextTrial();
		assertEqual("a", player.preparation().audioFilePath);
		prepareNextTrial();
		assertEqual("b", player.preparation().audioFilePath);
		prepareNextTrial();
		assertEqual("c", player.preparation().audioFilePath);
	}

	TEST_F(RecognitionTestTests, prepareNextTrialPassesParametersToPlayer) {
		nextTrial.audioDevice = "a";
		prepareNextTrial();
		assertEqual("a", player.preparation().audioDevice);
	}

	TEST_F(RecognitionTestTests, prepareNextTrialDoesNotAdvanceListWhenPlayerPlaying) {
		list.setContents({ "a", "b", "c" });
		prepareNextTrial();
		player.setPlaying();
		prepareNextTrial();
		assertEqual("a", player.preparation().audioFilePath);
	}

	TEST_F(RecognitionTestTests, prepareNextTrialDoesNotAdvanceListWhenPlayerFails) {
		list.setContents({ "a", "b", "c" });
		player.failOnPrepareToPlay();
		try {
			prepareNextTrial();
		}
		catch (const RecognitionTest::TrialFailure &) {

		}
		player.dontFailOnPrepareToPlay();
		prepareNextTrial();
		assertEqual("a", player.preparation().audioFilePath);
	}

	TEST_F(RecognitionTestTests, prepareNextTrialDoesNotPrepareAgainWhenPlayerAlreadyPlaying) {
		player.setPlaying();
		prepareNextTrial();
		EXPECT_FALSE(player.prepareToPlayCalled());
	}

	TEST_F(
		RecognitionTestTests,
		prepareNextTrialDocumentsTrial
	) {
		list.setContents({ "a", "b", "c" });
		nextTrial.level_dB_Spl = 1;
		prepareNextTrial();
		EXPECT_EQ(1, documenter.globalTrialParameters.level_dB_Spl);
		assertEqual("a", documenter.globalTrialParameters.stimulus);
	}

	TEST_F(RecognitionTestTests, playCalibrationPassesRequestToPlayer) {
		calibration.audioDevice = "a";
		calibration.audioFilePath = "b";
		playCalibration();
		assertEqual("a", player.preparation().audioDevice);
		assertEqual("b", player.preparation().audioFilePath);
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

		void assertPrepareNewTestThrowsInitializationFailure(std::string what) {
			try {
				model.prepareNewTest({});
				FAIL() << "Expected RecognitionTest::TestInitializationFailure";
			}
			catch (const RecognitionTest::TestInitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(
		RecognitionTestModelWithInitializationFailingDocumenter,
		prepareNewTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
	) {
		documenter.setErrorMessage("error.");
		assertPrepareNewTestThrowsInitializationFailure("error.");
	}

	class RecognitionTestModelWithRequestFailingStimulusPlayer : public ::testing::Test {
	protected:
		FakeStimulusList list{};
		RequestFailingStimulusPlayer player{};
		DocumenterStub documenter{};
		RecognitionTest model{ &list, &player, &documenter };

		void assertPrepareNextTrialThrowsTrialFailure(std::string what) {
			try {
				model.prepareNextTrial({});
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
		prepareNextTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
	) {
		player.setErrorMessage("error.");
		assertPrepareNextTrialThrowsTrialFailure("error.");
	}

	TEST_F(
		RecognitionTestModelWithRequestFailingStimulusPlayer,
		playCalibrationThrowsCalibrationFailureWhenPlayerThrowsRequestFailure
	) {
		player.setErrorMessage("error.");
		assertPlayCalibrationThrowsCalibrationFailure("error.");
	}
}