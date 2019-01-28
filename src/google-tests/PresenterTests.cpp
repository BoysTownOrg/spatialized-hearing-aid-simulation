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
			EXPECT_FALSE(view.testSetupHidden());
		}

		void confirmTestSetupHidesSetupView() {
			view.confirmTestSetup();
			EXPECT_TRUE(view.testSetupHidden());
		}

		void confirmTestSetupDoesNotShowTesterView() {
			view.confirmTestSetup();
			EXPECT_FALSE(view.testerViewShown());
		}

		void confirmTestSetupShowsTesterView() {
			view.confirmTestSetup();
			EXPECT_TRUE(view.testerViewShown());
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
			EXPECT_TRUE(view.brirFilePathDeactivated());
			EXPECT_TRUE(view.browseForBrirButtonDeactivated());
		}

		void assertSpatializationUIHasNotBeenActivated() {
			EXPECT_FALSE(view.brirFilePathActivated());
			EXPECT_FALSE(view.browseForBrirButtonActivated());
		}

		void assertSpatializationUIHasOnlyBeenActivated() {
			assertSpatializationUIHasBeenActivated();
			assertSpatializationUIHasNotBeenDeactivated();
		}

		void assertSpatializationUIHasBeenActivated() {
			EXPECT_TRUE(view.brirFilePathActivated());
			EXPECT_TRUE(view.browseForBrirButtonActivated());
		}

		void assertSpatializationUIHasNotBeenDeactivated() {
			EXPECT_FALSE(view.brirFilePathDeactivated());
			EXPECT_FALSE(view.browseForBrirButtonDeactivated());
		}

		void assertHearingAidUIHasOnlyBeenActivated() {
			assertHearingAidUIHasBeenActivated();
			assertHearingAidUIHasNotBeenDeactivated();
		}

		void assertHearingAidUIHasBeenActivated() {
			EXPECT_TRUE(view.leftDslPrescriptionFilePathActivated_);
			EXPECT_TRUE(view.rightDslPrescriptionFilePathActivated_);
			EXPECT_TRUE(view.browseForLeftDslPrescriptionButtonActivated_);
			EXPECT_TRUE(view.browseForRightDslPrescriptionButtonActivated_);
			EXPECT_TRUE(view.chunkSizeActivated_);
			EXPECT_TRUE(view.windowSizeActivated_);
			EXPECT_TRUE(view.attack_msActivated_);
			EXPECT_TRUE(view.release_msActivated_);
		}

		void assertHearingAidUIHasNotBeenDeactivated() {
			EXPECT_FALSE(view.leftDslPrescriptionFilePathDeactivated_);
			EXPECT_FALSE(view.rightDslPrescriptionFilePathDeactivated_);
			EXPECT_FALSE(view.browseForLeftDslPrescriptionButtonDeactivated_);
			EXPECT_FALSE(view.browseForRightDslPrescriptionButtonDeactivated_);
			EXPECT_FALSE(view.chunkSizeDeactivated_);
			EXPECT_FALSE(view.windowSizeDeactivated_);
			EXPECT_FALSE(view.attack_msDeactivated_);
			EXPECT_FALSE(view.release_msDeactivated_);
		}

		void assertHearingAidUIHasOnlyBeenDeactivated() {
			assertHearingAidUIHasNotBeenActivated();
			assertHearingAidUIHasBeenDeactivated();
		}

		void assertHearingAidUIHasNotBeenActivated() {
			EXPECT_FALSE(view.leftDslPrescriptionFilePathActivated_);
			EXPECT_FALSE(view.rightDslPrescriptionFilePathActivated_);
			EXPECT_FALSE(view.browseForLeftDslPrescriptionButtonActivated_);
			EXPECT_FALSE(view.browseForRightDslPrescriptionButtonActivated_);
			EXPECT_FALSE(view.chunkSizeActivated_);
			EXPECT_FALSE(view.windowSizeActivated_);
			EXPECT_FALSE(view.attack_msActivated_);
			EXPECT_FALSE(view.release_msActivated_);
		}

		void assertHearingAidUIHasBeenDeactivated() {
			EXPECT_TRUE(view.leftDslPrescriptionFilePathDeactivated_);
			EXPECT_TRUE(view.rightDslPrescriptionFilePathDeactivated_);
			EXPECT_TRUE(view.browseForLeftDslPrescriptionButtonDeactivated_);
			EXPECT_TRUE(view.browseForRightDslPrescriptionButtonDeactivated_);
			EXPECT_TRUE(view.chunkSizeDeactivated_);
			EXPECT_TRUE(view.windowSizeDeactivated_);
			EXPECT_TRUE(view.attack_msDeactivated_);
			EXPECT_TRUE(view.release_msDeactivated_);
		}
	};

	TEST_F(PresenterTests, subscribesToViewEvents) {
		EXPECT_EQ(&presenter, view.listener());
	}

	TEST_F(PresenterTests, runRunsEventLoop) {
		presenter.run();
		EXPECT_TRUE(view.runningEventLoop());
	}

	TEST_F(PresenterTests, constructorShowsTestSetupView) {
		EXPECT_TRUE(view.testSetupShown());
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
		EXPECT_TRUE(view.testerViewShown());
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
		assertEqual("a", model.globalTestParameters.leftDslPrescriptionFilePath);
		assertEqual("b", model.globalTestParameters.rightDslPrescriptionFilePath);
		assertEqual("c", model.testParameters().audioDirectory);
		assertEqual("d", model.globalTestParameters.brirFilePath);
		assertEqual("e", model.testParameters().testFilePath);
		assertEqual("f", model.globalTestParameters.subjectId);
		assertEqual("g", model.globalTestParameters.testerId);
		EXPECT_EQ(2.2, model.globalTestParameters.attack_ms);
		EXPECT_EQ(3.3, model.globalTestParameters.release_ms);
		EXPECT_EQ(4, model.globalTestParameters.windowSize);
		EXPECT_EQ(5, model.globalTestParameters.chunkSize);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingSpatialization
	) {
		view.setSpatializationOn();
		view.confirmTestSetup();
		EXPECT_TRUE(model.globalTestParameters.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingSpatialization
	) {
		view.setSpatializationOff();
		view.confirmTestSetup();
		EXPECT_FALSE(model.globalTestParameters.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOn();
		view.confirmTestSetup();
		EXPECT_TRUE(model.globalTestParameters.usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOff();
		view.confirmTestSetup();
		EXPECT_FALSE(model.globalTestParameters.usingHearingAidSimulation);
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
		EXPECT_TRUE(model.trialPlayed());
	}

	TEST_F(PresenterTests, playingTrialDoesNotHideTesterViewWhileTestInProgress) {
		model.setTestIncomplete();
		view.playTrial();
		EXPECT_FALSE(view.testerViewHidden());
	}

	TEST_F(PresenterTests, playingLastTrialHidesTesterViewAndShowsSetupView) {
		model.setTestComplete();
		view.playTrial();
		EXPECT_TRUE(view.testerViewHidden());
		EXPECT_TRUE(view.testSetupShown());
	}

	TEST_F(PresenterTests, playTrialPassesParametersToModel) {
		view.setAudioDevice("e");
		view.setLevel_dB_Spl("1.1");
		view.playTrial();
		assertEqual("e", model.trialParameters().audioDevice);
		EXPECT_EQ(1.1, model.trialParameters().level_dB_Spl);
	}

	TEST_F(PresenterTests, calibrateShowsCalibrationView) {
		view.calibrate();
		EXPECT_TRUE(view.calibrationShown());
	}

	TEST_F(PresenterTests, playCalibrationPlaysCalibration) {
		view.playCalibration();
		EXPECT_TRUE(model.calibrationPlayed());
	}

	TEST_F(PresenterTests, stopCalibrationStopsCalibration) {
		view.stopCalibration();
		EXPECT_TRUE(model.calibrationStopped());
	}

	TEST_F(PresenterTests, confirmCalibrationPassesCalibratedLevel) {
		view.setCalibrationLevel_dB_Spl(1);
		view.confirmCalibration();
		EXPECT_EQ(1, model.calibrationLevel_dB_Spl());
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
		EXPECT_FALSE(view.testerViewShown());
	}

	TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupDoesNotHideSetupView) {
		view.confirmTestSetup();
		EXPECT_FALSE(view.testSetupHidden());
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
}