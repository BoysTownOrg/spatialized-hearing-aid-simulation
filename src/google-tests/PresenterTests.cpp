#include "assert-utility.h"
#include "ModelStub.h"
#include "ViewStub.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
	class PresenterTests : public ::testing::Test {
	protected:
		ModelStub model;
		ViewStub view;
		Presenter presenter{ &model, &view };

		void assertCancellingBrowseDoesNotChangePath(
			std::string expected, 
			std::string &path, 
			void(ViewStub::*browse)()
		) {
			view.setBrowseCancelled();
			(view.*browse)();
			assertEqual(expected, path);
		}

		void setInvalidChunkSize() {
			view.setChunkSize("?");
		}

		void setInvalidWindowSize() {
			view.setWindowSize("a");
		}

		void setInvalidReleaseTime() {
			view.setRelease_ms("b");
		}

		void setInvalidAttackTime() {
			view.setAttack_ms("c");
		}

		void setInvalidLevel() {
			view.setLevel_dB_Spl("d");
		}

		void confirmTestSetupDoesNotHideSetupView() {
			view.confirmTestSetup();
			assertFalse(view.testSetupHidden());
		}

		void confirmTestSetupHidesSetupView() {
			view.confirmTestSetup();
			assertTrue(view.testSetupHidden());
		}

		void confirmTestSetupDoesNotShowTesterView() {
			view.confirmTestSetup();
			assertFalse(view.testerViewShown());
		}

		void confirmTestSetupShowsTesterView() {
			view.confirmTestSetup();
			assertTrue(view.testerViewShown());
		}

		void confirmTestSetupWithChunkSizeShowsErrorMessage(std::string s) {
			view.setChunkSize(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid chunk size.");
		}

		void confirmTestSetupShowsErrorMessage(std::string s) {
			view.confirmTestSetup();
			assertEqual(std::move(s), view.errorMessage());
		}

		void confirmTestSetupWithWindowSizeShowsErrorMessage(std::string s) {
			view.setWindowSize(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid window size.");
		}

		void confirmTestSetupWithAttackTimeShowsErrorMessage(std::string s) {
			view.setAttack_ms(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}

		void confirmTestSetupWithReleaseTimeShowsErrorMessage(std::string s) {
			view.setRelease_ms(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid release time.");
		}

		void playCalibrationWithAttackTimeShowsErrorMessage(std::string s) {
			view.setAttack_ms(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}

		void playCalibrationWithReleaseTimeShowsErrorMessage(std::string s) {
			view.setRelease_ms(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid release time.");
		}

		void playCalibrationWithLevelShowsErrorMessage(std::string s) {
			view.setLevel_dB_Spl(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void playCalibrationShowsErrorMessage(std::string s) {
			view.playCalibration();
			assertEqual(std::move(s), view.errorMessage());
		}

		void playTrialWithLevelShowsErrorMessage(std::string s) {
			view.setLevel_dB_Spl(s);
			playTrialShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void playTrialShowsErrorMessage(std::string s) {
			view.playTrial();
			assertEqual(std::move(s), view.errorMessage());
		}

		void assertSpatializationUIHasOnlyBeenDeactivated() {
			assertSpatializationUIHasBeenDeactivated();
			assertSpatializationUIHasNotBeenActivated();
		}

		void assertSpatializationUIHasBeenDeactivated() {
			assertTrue(view.brirFilePathDeactivated());
			assertTrue(view.browseForBrirButtonDeactivated());
		}

		void assertSpatializationUIHasNotBeenActivated() {
			assertFalse(view.brirFilePathActivated());
			assertFalse(view.browseForBrirButtonActivated());
		}

		void assertSpatializationUIHasOnlyBeenActivated() {
			assertSpatializationUIHasBeenActivated();
			assertSpatializationUIHasNotBeenDeactivated();
		}

		void assertSpatializationUIHasBeenActivated() {
			assertTrue(view.brirFilePathActivated());
			assertTrue(view.browseForBrirButtonActivated());
		}

		void assertSpatializationUIHasNotBeenDeactivated() {
			assertFalse(view.brirFilePathDeactivated());
			assertFalse(view.browseForBrirButtonDeactivated());
		}

		void assertHearingAidUIHasOnlyBeenActivated() {
			assertHearingAidUIHasBeenActivated();
			assertHearingAidUIHasNotBeenDeactivated();
		}

		void assertHearingAidUIHasBeenActivated() {
			assertTrue(view.leftDslPrescriptionFilePathActivated_);
			assertTrue(view.rightDslPrescriptionFilePathActivated_);
			assertTrue(view.browseForLeftDslPrescriptionButtonActivated_);
			assertTrue(view.browseForRightDslPrescriptionButtonActivated_);
			assertTrue(view.chunkSizeActivated_);
			assertTrue(view.windowSizeActivated_);
			assertTrue(view.attack_msActivated_);
			assertTrue(view.release_msActivated_);
		}

		void assertHearingAidUIHasNotBeenDeactivated() {
			assertFalse(view.leftDslPrescriptionFilePathDeactivated_);
			assertFalse(view.rightDslPrescriptionFilePathDeactivated_);
			assertFalse(view.browseForLeftDslPrescriptionButtonDeactivated_);
			assertFalse(view.browseForRightDslPrescriptionButtonDeactivated_);
			assertFalse(view.chunkSizeDeactivated_);
			assertFalse(view.windowSizeDeactivated_);
			assertFalse(view.attack_msDeactivated_);
			assertFalse(view.release_msDeactivated_);
		}

		void assertHearingAidUIHasOnlyBeenDeactivated() {
			assertHearingAidUIHasNotBeenActivated();
			assertHearingAidUIHasBeenDeactivated();
		}

		void assertHearingAidUIHasNotBeenActivated() {
			assertFalse(view.leftDslPrescriptionFilePathActivated_);
			assertFalse(view.rightDslPrescriptionFilePathActivated_);
			assertFalse(view.browseForLeftDslPrescriptionButtonActivated_);
			assertFalse(view.browseForRightDslPrescriptionButtonActivated_);
			assertFalse(view.chunkSizeActivated_);
			assertFalse(view.windowSizeActivated_);
			assertFalse(view.attack_msActivated_);
			assertFalse(view.release_msActivated_);
		}

		void assertHearingAidUIHasBeenDeactivated() {
			assertTrue(view.leftDslPrescriptionFilePathDeactivated_);
			assertTrue(view.rightDslPrescriptionFilePathDeactivated_);
			assertTrue(view.browseForLeftDslPrescriptionButtonDeactivated_);
			assertTrue(view.browseForRightDslPrescriptionButtonDeactivated_);
			assertTrue(view.chunkSizeDeactivated_);
			assertTrue(view.windowSizeDeactivated_);
			assertTrue(view.attack_msDeactivated_);
			assertTrue(view.release_msDeactivated_);
		}
	};

	TEST_F(PresenterTests, subscribesToViewEvents) {
		EXPECT_EQ(&presenter, view.listener());
	}

	TEST_F(PresenterTests, runRunsEventLoop) {
		presenter.run();
		assertTrue(view.runningEventLoop());
	}

	TEST_F(PresenterTests, constructorShowsTestSetupView) {
		assertTrue(view.testSetupShown());
	}

	TEST_F(PresenterTests, constructorPopulatesAudioDeviceMenu) {
		model.setAudioDeviceDescriptions({ "a", "b", "c" });
		Presenter{ &model, &view };
		assertEqual({ "a", "b", "c" }, view.audioDeviceMenuItems());
	}

	TEST_F(PresenterTests, constructorActivatesSpatializationUIWhenInitiallyOn) {
		view.clearActivationState();
		view.setSpatializationOn();
		Presenter{ &model, &view };
		assertSpatializationUIHasOnlyBeenActivated();
	}

	TEST_F(PresenterTests, constructordeactivatesSpatializationUIWhenInitiallyOff) {
		view.clearActivationState();
		view.setSpatializationOff();
		Presenter{ &model, &view };
		assertSpatializationUIHasOnlyBeenDeactivated();
	}

	TEST_F(PresenterTests, constructorActivatesHearingAidSimulationUIWhenInitiallyOn) {
		view.clearActivationState();
		view.setHearingAidSimulationOn();
		Presenter{ &model, &view };
		assertHearingAidUIHasOnlyBeenActivated();
	}

	TEST_F(PresenterTests, constructorDeactivatesHearingAidSimulationUIWhenInitiallyOff) {
		view.clearActivationState();
		view.setHearingAidSimulationOff();
		Presenter{ &model, &view };
		assertHearingAidUIHasOnlyBeenDeactivated();
	}

	TEST_F(PresenterTests, confirmTestSetupShowsTesterView) {
		view.confirmTestSetup();
		assertTrue(view.testerViewShown());
	}

	TEST_F(PresenterTests, confirmTestSetupHidesTestSetupView) {
		confirmTestSetupHidesSetupView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidChunkSizeButNoHearingAidSimulationStillHidesSetupView
	) {
		view.setHearingAidSimulationOff();
		setInvalidChunkSize();
		confirmTestSetupHidesSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotHideSetupView) {
		view.setHearingAidSimulationOn();
		setInvalidChunkSize();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidWindowSizeButNoHearingAidSimulationStillHidesSetupView
	) {
		view.setHearingAidSimulationOff();
		setInvalidWindowSize();
		confirmTestSetupHidesSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotHideSetupView) {
		view.setHearingAidSimulationOn();
		setInvalidWindowSize();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidReleaseTimeButNoHearingAidSimulationStillHidesSetupView
	) {
		view.setHearingAidSimulationOff();
		setInvalidReleaseTime();
		confirmTestSetupHidesSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotHideSetupView) {
		view.setHearingAidSimulationOn();
		setInvalidReleaseTime();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidAttackTimeButNoHearingAidSimulationStillHidesSetupView
	) {
		view.setHearingAidSimulationOff();
		setInvalidAttackTime();
		confirmTestSetupHidesSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotHideSetupView) {
		view.setHearingAidSimulationOn();
		setInvalidAttackTime();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidChunkSizeButNoHearingAidSimulationShowsTesterView
	) {
		view.setHearingAidSimulationOff();
		setInvalidChunkSize();
		confirmTestSetupShowsTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotShowTesterView) {
		view.setHearingAidSimulationOn();
		setInvalidChunkSize();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidWindowSizeButNoHearingAidSimulationShowsTesterView
	) {
		view.setHearingAidSimulationOff();
		setInvalidWindowSize();
		confirmTestSetupShowsTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotShowTesterView) {
		view.setHearingAidSimulationOn();
		setInvalidWindowSize();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidReleaseTimeButNoHearingAidSimulationShowsTesterView
	) {
		view.setHearingAidSimulationOff();
		setInvalidReleaseTime();
		confirmTestSetupShowsTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotShowTesterView) {
		view.setHearingAidSimulationOn();
		setInvalidReleaseTime();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(
		PresenterTests, 
		confirmTestSetupWithInvalidAttackTimeButNoHearingAidSimulationShowsTesterView
	) {
		view.setHearingAidSimulationOff();
		setInvalidAttackTime();
		confirmTestSetupShowsTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotShowTesterView) {
		view.setHearingAidSimulationOn();
		setInvalidAttackTime();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			confirmTestSetupWithChunkSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			confirmTestSetupWithWindowSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		confirmTestSetupWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		confirmTestSetupWithReleaseTimeShowsErrorMessage("b");
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupPassesParametersToModel
	) {
		view.setHearingAidSimulationOn();
		view.setLeftDslPrescriptionFilePath("a");
		view.setRightDslPrescriptionFilePath("b");
		view.setAudioDirectory("c");
		view.setBrirFilePath("d");
		view.setTestFilePath("e");
		view.setSubjectId("f");
		view.setTesterId("g");
		view.setAttack_ms("2.2");
		view.setRelease_ms("3.3");
		view.setWindowSize("4");
		view.setChunkSize("5");
		view.confirmTestSetup();
		assertEqual("a", model.testParameters().leftDslPrescriptionFilePath);
		assertEqual("b", model.testParameters().rightDslPrescriptionFilePath);
		assertEqual("c", model.testParameters().audioDirectory);
		assertEqual("d", model.testParameters().brirFilePath);
		assertEqual("e", model.testParameters().testFilePath);
		assertEqual("f", model.testParameters().subjectId);
		assertEqual("g", model.testParameters().testerId);
		assertEqual(2.2, model.testParameters().attack_ms);
		assertEqual(3.3, model.testParameters().release_ms);
		assertEqual(4, model.testParameters().windowSize);
		assertEqual(5, model.testParameters().chunkSize);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingSpatialization
	) {
		view.setSpatializationOn();
		view.confirmTestSetup();
		assertTrue(model.testParameters().usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingSpatialization
	) {
		view.setSpatializationOff();
		view.confirmTestSetup();
		assertFalse(model.testParameters().usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOn();
		view.confirmTestSetup();
		assertTrue(model.testParameters().usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOff();
		view.confirmTestSetup();
		assertFalse(model.testParameters().usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		cancellingBrowseForTestFileDoesNotChangeTestFilePath
	) {
		view.testFilePath_ = "a";
		assertCancellingBrowseDoesNotChangePath("a", view.testFilePath_, &ViewStub::browseForTestFile);
	}

	TEST_F(
		PresenterTests,
		cancellingBrowseForDslPrescriptionDoesNotChangeDslPrescriptionFilePath
	) {
		view.leftDslPrescriptionFilePath_ = "a";
		view.rightDslPrescriptionFilePath_ = "b";
		assertCancellingBrowseDoesNotChangePath(
			"a", 
			view.leftDslPrescriptionFilePath_, 
			&ViewStub::browseForLeftDslPrescription
		);
		assertCancellingBrowseDoesNotChangePath(
			"b", 
			view.rightDslPrescriptionFilePath_, 
			&ViewStub::browseForRightDslPrescription
		);
	}

	TEST_F(
		PresenterTests,
		cancellingBrowseForAudioDirectoryNotChangeAudioDirectory
	) {
		view.audioDirectory_ = "a";
		assertCancellingBrowseDoesNotChangePath("a", view.audioDirectory_, &ViewStub::browseForAudio);
	}


	TEST_F(
		PresenterTests,
		cancellingBrowseForBrirDoesNotChangeBrirFilePath
	) {
		view.brirFilePath_ = "a";
		assertCancellingBrowseDoesNotChangePath("a", view.brirFilePath_, &ViewStub::browseForBrir);
	}

	TEST_F(
		PresenterTests,
		browseForTestFileFiltersTextFiles
	) {
		view.browseForTestFile();
		assertEqual({ "*.txt" }, view.browseFilters());
	}

	TEST_F(
		PresenterTests,
		browseForBrirFiltersWavFiles
	) {
		view.browseForBrir();
		assertEqual({ "*.wav" }, view.browseFilters());
	}

	TEST_F(
		PresenterTests,
		browseForTestFileUpdatesTestFilePath
	) {
		view.setBrowseFilePath("a");
		view.browseForTestFile();
		assertEqual("a", view.testFilePath());
	}

	TEST_F(
		PresenterTests,
		browseForDslPrescriptionUpdatesDslPrescriptionFilePath
	) {
		view.setBrowseFilePath("a");
		view.browseForLeftDslPrescription();
		assertEqual("a", view.leftDslPrescriptionFilePath());
		view.setBrowseFilePath("b");
		view.browseForRightDslPrescription();
		assertEqual("b", view.rightDslPrescriptionFilePath());
	}

	TEST_F(
		PresenterTests,
		browseForAudioDirectoryUpdatesAudioDirectory
	) {
		view.setBrowseDirectory("a");
		view.browseForAudio();
		assertEqual("a", view.audioDirectory());
	}

	TEST_F(
		PresenterTests,
		browseForBrirUpdatesBrirFilePath
	) {
		view.setBrowseFilePath("a");
		view.browseForBrir();
		assertEqual("a", view.brirFilePath());
	}

	TEST_F(PresenterTests, togglingSpatializationOffDeactivatesUI) {
		view.clearActivationState();
		view.setSpatializationOff();
		view.toggleSpatialization();
		assertSpatializationUIHasOnlyBeenDeactivated();
	}

	TEST_F(PresenterTests, togglingSpatializationOnActivatesUI) {
		view.clearActivationState();
		view.setSpatializationOn();
		view.toggleSpatialization();
		assertSpatializationUIHasOnlyBeenActivated();
	}

	TEST_F(PresenterTests, togglingHearingAidSimulationOffDeactivatesUI) {
		view.clearActivationState();
		view.setHearingAidSimulationOn();
		view.toggleHearingAidSimulation();
		assertHearingAidUIHasOnlyBeenActivated();
	}

	TEST_F(PresenterTests, togglingHearingAidSimulationOnActivatesUI) {
		view.clearActivationState();
		view.setHearingAidSimulationOff();
		view.toggleHearingAidSimulation();
		assertHearingAidUIHasOnlyBeenDeactivated();
	}

	TEST_F(PresenterTests, playTrialWithInvalidLevelShowsErrorMessage) {
		playTrialWithLevelShowsErrorMessage("b");
	}

	TEST_F(PresenterTests, playingTrialPlaysTrial) {
		view.playTrial();
		assertTrue(model.trialPlayed());
	}

	TEST_F(PresenterTests, playingTrialWithInvalidLevelDoesNotPlayTrial) {
		setInvalidLevel();
		view.playTrial();
		assertFalse(model.trialPlayed());
	}

	TEST_F(PresenterTests, playingTrialDoesNotHideTesterViewWhileTestInProgress) {
		model.setTestIncomplete();
		view.playTrial();
		assertFalse(view.testerViewHidden());
	}

	TEST_F(PresenterTests, playingLastTrialHidesTesterViewAndShowsSetupView) {
		model.setTestComplete();
		view.playTrial();
		assertTrue(view.testerViewHidden());
		assertTrue(view.testSetupShown());
	}

	TEST_F(PresenterTests, playTrialPassesParametersToModel) {
		view.setAudioDevice("e");
		view.setLevel_dB_Spl("1.1");
		view.playTrial();
		assertEqual("e", model.trialParameters().audioDevice);
		assertEqual(1.1, model.trialParameters().level_dB_Spl);
	}

	TEST_F(PresenterTests, calibrateShowsCalibrationView) {
		view.calibrate();
		assertTrue(view.calibrationShown());
	}

	TEST_F(PresenterTests, playCalibrationPassesCalibrationParametersToModel) {
		view.setAudioDevice("a");
		view.setAudioFilePath("b");
		view.setLevel_dB_Spl("1.1");
		view.playCalibration();
		assertEqual("a", model.calibrationParameters().audioDevice);
		assertEqual("b", model.calibrationParameters().audioFilePath);
		assertEqual(1.1, model.calibrationParameters().level_dB_Spl);
	}

	TEST_F(PresenterTests, playCalibrationWithHearingAidSimulationPassesParametersToModel) {
		view.setHearingAidSimulationOn();
		view.setAttack_ms("1.0");
		view.setRelease_ms("2.0");
		view.playCalibration();
		assertEqual(1.0, model.calibrationParameters().processing.attack_ms);
		assertEqual(2.0, model.calibrationParameters().processing.release_ms);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidLevelShowsErrorMessage) {
		playCalibrationWithLevelShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidLevelDoesNotPlay) {
		setInvalidLevel();
		view.playCalibration();
		assertFalse(model.calibrationPlayed());
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		playCalibrationWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackDoesNotPlay) {
		view.setHearingAidSimulationOn();
		setInvalidAttackTime();
		view.playCalibration();
		assertFalse(model.calibrationPlayed());
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackButNoHearingAidSimulationStillPlays) {
		view.setHearingAidSimulationOff();
		setInvalidAttackTime();
		view.playCalibration();
		assertTrue(model.calibrationPlayed());
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidReleaseShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		playCalibrationWithReleaseTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidReleaseDoesNotPlay) {
		view.setHearingAidSimulationOn();
		setInvalidReleaseTime();
		view.playCalibration();
		assertFalse(model.calibrationPlayed());
	}

	TEST_F(PresenterTests, stopCalibrationStopsCalibration) {
		view.stopCalibration();
		assertTrue(model.calibrationStopped());
	}

	class PresenterWithInitializationFailingModel : public ::testing::Test {
	protected:
		InitializationFailingModel model;
		ViewStub view;
		Presenter presenter{ &model, &view };
	};

	TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupShowsErrorMessage) {
		model.setErrorMessage("error.");
		view.confirmTestSetup();
		assertEqual("error.", view.errorMessage());
	}

	TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupDoesNotShowTesterView) {
		view.confirmTestSetup();
		assertFalse(view.testerViewShown());
	}

	TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupDoesNotHideSetupView) {
		view.confirmTestSetup();
		assertFalse(view.testSetupHidden());
	}

	class PresenterWithTrialFailingModel : public ::testing::Test {
	protected:
		TrialFailingModel model;
		ViewStub view;
		Presenter presenter{ &model, &view };
	};

	TEST_F(PresenterWithTrialFailingModel, playTrialShowsErrorMessage) {
		model.setErrorMessage("error.");
		view.playTrial();
		assertEqual("error.", view.errorMessage());
	}

	class PresenterWithCalibrationFailingModel : public ::testing::Test {
	protected:
		CalibrationFailingModel model;
		ViewStub view;
		Presenter presenter{ &model, &view };
	};

	TEST_F(PresenterWithCalibrationFailingModel, playCalibrationShowsErrorMessage) {
		model.setErrorMessage("error.");
		view.playCalibration();
		assertEqual("error.", view.errorMessage());
	}
}