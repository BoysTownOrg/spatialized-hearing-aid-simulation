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
			view.setChunkSize("a");
		}

		void setInvalidWindowSize() {
			view.setWindowSize("a");
		}

		void setInvalidReleaseTime() {
			view.setRelease_ms("a");
		}

		void setInvalidAttackTime() {
			view.setAttack_ms("a");
		}

		void setInvalidLevel() {
			view.setLevel_dB_Spl("a");
		}

		void assertTesterViewHidden() {
			assertTrue(view.everyItemInTesterViewHidden());
		}

		void assertTestSetupViewShown() {
			assertTrue(view.everyItemInTestSetupShown());
		}

		void assertTestSetupViewNotHidden() {
			assertTrue(view.notASingleElementInTestSetupHidden());
		}

		void assertTestSetupViewHidden() {
			assertTrue(view.everyItemInTestSetupHidden());
		}

		void assertTestSetupViewNotShown() {
			assertTrue(view.notASingleElementInTesterViewShown());
		}

		void assertTesterViewShown() {
			assertTrue(view.everyItemInTesterViewShown());
		}

		void confirmTestSetupDoesNotHideSetupView() {
			auto assertTestSetupViewNotHidden = &PresenterTests::assertTestSetupViewNotHidden;
			confirmTestSetupThen(assertTestSetupViewNotHidden);
		}

		template<typename... Targs>
		void confirmTestSetupThen(void(PresenterTests::*f)(Targs...), Targs&&... args) {
			confirmTestSetup();
			(this->*f)(std::forward<Targs>(args)...);
		}

		void confirmTestSetup() {
			view.confirmTestSetup();
		}

		void confirmTestSetupHidesSetupView() {
			auto assertTestSetupViewHidden = &PresenterTests::assertTestSetupViewHidden;
			confirmTestSetupThen(assertTestSetupViewHidden);
		}

		void confirmTestSetupDoesNotShowTesterView() {
			auto assertTestSetupViewNotShown = &PresenterTests::assertTestSetupViewNotShown;
			confirmTestSetupThen(assertTestSetupViewNotShown);
		}

		void confirmTestSetupShowsTesterView() {
			auto assertTesterViewShown = &PresenterTests::assertTesterViewShown;
			confirmTestSetupThen(assertTesterViewShown);
		}

		void confirmTestSetupDoesNotPrepareTest() {
			auto assertTestHasNotBeenPrepared = &PresenterTests::assertTestHasNotBeenPrepared;
			confirmTestSetupThen(assertTestHasNotBeenPrepared);
		}

		void assertTestHasNotBeenPrepared() {
			assertFalse(model.testPrepared());
		}

		void confirmTestSetupWithChunkSizeShowsErrorMessage(std::string s) {
			view.setChunkSize(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid chunk size.");
		}

		void confirmTestSetupShowsErrorMessage(std::string s) {
			auto assertErrorMessageEquals = &PresenterTests::assertErrorMessageEquals;
			confirmTestSetupThen(assertErrorMessageEquals, std::move(s));
		}

		void assertErrorMessageEquals(std::string s) {
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

		void playCalibrationDoesNotPlay() {
			view.playCalibration();
			assertFalse(model.calibrationPlayed());
		}

		void playCalibrationPlays() {
			view.playCalibration();
			assertTrue(model.calibrationPlayed());
		}

		void saveAudioSaves() {
			view.processAudioForSaving();
			assertTrue(model.audioSaved());
		}
		
		void playCalibrationWithAttackTimeShowsErrorMessage(std::string s) {
			view.setAttack_ms(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}

		void saveAudioWithAttackTimeShowsErrorMessage(std::string s) {
			view.setAttack_ms(s);
			saveAudioShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}
		
		void playCalibrationWithReleaseTimeShowsErrorMessage(std::string s) {
			view.setRelease_ms(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid release time.");
		}
		
		void saveAudioWithReleaseTimeShowsErrorMessage(std::string s) {
			view.setRelease_ms(s);
			saveAudioShowsErrorMessage("'" + s + "' is not a valid release time.");
		}
		
		void playCalibrationWithChunkSizeShowsErrorMessage(std::string s) {
			view.setChunkSize(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid chunk size.");
		}

		void playCalibrationWithWindowSizeShowsErrorMessage(std::string s) {
			view.setWindowSize(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid window size.");
		}

		void playCalibrationWithLevelShowsErrorMessage(std::string s) {
			view.setLevel_dB_Spl(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void saveAudioWithLevelShowsErrorMessage(std::string s) {
			view.setLevel_dB_Spl(s);
			saveAudioShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void playCalibrationShowsErrorMessage(std::string s) {
			view.playCalibration();
			assertErrorMessageEquals(std::move(s));
		}

		void saveAudioShowsErrorMessage(std::string s) {
			view.processAudioForSaving();
			assertErrorMessageEquals(std::move(s));
		}
		
		void playTrialWithLevelShowsErrorMessage(std::string s) {
			view.setLevel_dB_Spl(s);
			playTrialShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void playTrialShowsErrorMessage(std::string s) {
			view.playNextTrial();
			assertErrorMessageEquals(std::move(s));
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
		assertTestSetupViewShown();
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
		confirmTestSetupShowsTesterView();
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

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotPrepareTest) {
		view.setHearingAidSimulationOn();
		setInvalidChunkSize();
		confirmTestSetupDoesNotPrepareTest();
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

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotPrepareTest) {
		view.setHearingAidSimulationOn();
		setInvalidWindowSize();
		confirmTestSetupDoesNotPrepareTest();
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

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotPrepareTest) {
		view.setHearingAidSimulationOn();
		setInvalidReleaseTime();
		confirmTestSetupDoesNotPrepareTest();
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

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotPrepareTest) {
		view.setHearingAidSimulationOn();
		setInvalidAttackTime();
		confirmTestSetupDoesNotPrepareTest();
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
		view.setStimulusList("a");
		view.setTestFilePath("b");
		view.setSubjectId("c");
		view.setTesterId("d");
		view.confirmTestSetup();
		assertEqual("a", model.testParameters().audioDirectory);
		assertEqual("b", model.testParameters().testFilePath);
		assertEqual("c", model.testParameters().subjectId);
		assertEqual("d", model.testParameters().testerId);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupPassesHearingAidParametersToModel
	) {
		view.setHearingAidSimulationOn();
		view.setLeftDslPrescriptionFilePath("a");
		view.setRightDslPrescriptionFilePath("b");
		view.setAttack_ms("2.2");
		view.setRelease_ms("3.3");
		view.setWindowSize("4");
		view.setChunkSize("5");
		view.confirmTestSetup();
		assertEqual("a", model.testParameters().processing.leftDslPrescriptionFilePath);
		assertEqual("b", model.testParameters().processing.rightDslPrescriptionFilePath);
		assertEqual(2.2, model.testParameters().processing.attack_ms);
		assertEqual(3.3, model.testParameters().processing.release_ms);
		assertEqual(4, model.testParameters().processing.windowSize);
		assertEqual(5, model.testParameters().processing.chunkSize);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupPassesSpatializationParametersToModel
	) {
		view.setSpatializationOn();
		view.setBrirFilePath("a");
		view.confirmTestSetup();
		assertEqual("a", model.testParameters().processing.brirFilePath);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingSpatialization
	) {
		view.setSpatializationOn();
		view.confirmTestSetup();
		assertTrue(model.testParameters().processing.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingSpatialization
	) {
		view.setSpatializationOff();
		view.confirmTestSetup();
		assertFalse(model.testParameters().processing.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOn();
		view.confirmTestSetup();
		assertTrue(model.testParameters().processing.usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOff();
		view.confirmTestSetup();
		assertFalse(model.testParameters().processing.usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		cancellingBrowseForAudioFileDoesNotChangeAudioFilePath
	) {
		view.audioFilePath_ = "a";
		assertCancellingBrowseDoesNotChangePath("a", view.audioFilePath_, &ViewStub::browseForAudioFile);
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
		cancellingBrowseForStimulusListNotChangeStimulusList
	) {
		view.stimulusList_ = "a";
		assertCancellingBrowseDoesNotChangePath("a", view.stimulusList_, &ViewStub::browseForStimulusList);
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
		assertEqual({ "*.txt" }, view.browseFiltersForSavingFile());
	}

	TEST_F(
		PresenterTests,
		browseForAudioFileFiltersAudioFiles
	) {
		view.browseForAudioFile();
		assertEqual({ "*.wav" }, view.browseForOpeningFileFilters());
	}

	TEST_F(
		PresenterTests,
		browseForBrirFiltersWavFiles
	) {
		view.browseForBrir();
		assertEqual({ "*.wav" }, view.browseForOpeningFileFilters());
	}

	TEST_F(
		PresenterTests,
		browseForTestFileUpdatesTestFilePath
	) {
		view.setBrowseForSavingFileResult("a");
		view.browseForTestFile();
		assertEqual("a", view.testFilePath());
	}

	TEST_F(
		PresenterTests,
		browseForAudioFileUpdatesAudioFilePath
	) {
		view.setBrowseForOpeningFileResult("a");
		view.browseForAudioFile();
		assertEqual("a", view.audioFilePath());
	}

	TEST_F(
		PresenterTests,
		browseForDslPrescriptionUpdatesDslPrescriptionFilePath
	) {
		view.setBrowseForOpeningFileResult("a");
		view.browseForLeftDslPrescription();
		assertEqual("a", view.leftDslPrescriptionFilePath());
		view.setBrowseForOpeningFileResult("b");
		view.browseForRightDslPrescription();
		assertEqual("b", view.rightDslPrescriptionFilePath());
	}

	TEST_F(
		PresenterTests,
		browseForStimulusListUpdatesStimulusList
	) {
		view.setBrowseDirectory("a");
		view.browseForStimulusList();
		assertEqual("a", view.stimulusList());
	}

	TEST_F(
		PresenterTests,
		browseForBrirUpdatesBrirFilePath
	) {
		view.setBrowseForOpeningFileResult("a");
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
		view.playNextTrial();
		assertTrue(model.trialPlayed());
	}

	TEST_F(PresenterTests, playingTrialWithInvalidLevelDoesNotPlayTrial) {
		setInvalidLevel();
		view.playNextTrial();
		assertFalse(model.trialPlayed());
	}

	TEST_F(PresenterTests, playingTrialDoesNotHideTesterViewWhileTestInProgress) {
		model.setTestIncomplete();
		view.playNextTrial();
		assertFalse(view.testerViewHidden());
	}

	TEST_F(PresenterTests, playingLastTrialHidesTesterViewAndShowsSetupView) {
		model.setTestComplete();
		view.playNextTrial();
		assertTesterViewHidden();
		assertTestSetupViewShown();
	}

	TEST_F(PresenterTests, playTrialPassesParametersToModel) {
		view.setAudioDevice("e");
		view.setLevel_dB_Spl("1.1");
		view.playNextTrial();
		assertEqual("e", model.trialParameters().audioDevice);
		assertEqual(1.1, model.trialParameters().level_dB_Spl);
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

	TEST_F(PresenterTests, saveAudioPassesParametersToModel) {
		view.setAudioFilePath("a");
		view.setLevel_dB_Spl("1.1");
		view.processAudioForSaving();
		assertEqual("a", model.saveAudioParameters().inputAudioFilePath);
		assertEqual(1.1, model.saveAudioParameters().level_dB_Spl);
	}

	TEST_F(PresenterTests, playCalibrationWithHearingAidSimulationPassesParametersToModel) {
		view.setHearingAidSimulationOn();
		view.setAttack_ms("1.1");
		view.setRelease_ms("2.2");
		view.setChunkSize("3");
		view.setWindowSize("4");
		view.setLeftDslPrescriptionFilePath("a");
		view.setRightDslPrescriptionFilePath("b");
		view.playCalibration();
		assertEqual(1.1, model.calibrationParameters().processing.attack_ms);
		assertEqual(2.2, model.calibrationParameters().processing.release_ms);
		assertEqual(3, model.calibrationParameters().processing.chunkSize);
		assertEqual(4, model.calibrationParameters().processing.windowSize);
		assertEqual("a", model.calibrationParameters().processing.leftDslPrescriptionFilePath);
		assertEqual("b", model.calibrationParameters().processing.rightDslPrescriptionFilePath);
	}

	TEST_F(PresenterTests, saveAudioWithHearingAidSimulationPassesParametersToModel) {
		view.setHearingAidSimulationOn();
		view.setAttack_ms("1.1");
		view.setRelease_ms("2.2");
		view.setChunkSize("3");
		view.setWindowSize("4");
		view.setLeftDslPrescriptionFilePath("a");
		view.setRightDslPrescriptionFilePath("b");
		view.processAudioForSaving();
		assertEqual(1.1, model.saveAudioParameters().processing.attack_ms);
		assertEqual(2.2, model.saveAudioParameters().processing.release_ms);
		assertEqual(3, model.saveAudioParameters().processing.chunkSize);
		assertEqual(4, model.saveAudioParameters().processing.windowSize);
		assertEqual("a", model.saveAudioParameters().processing.leftDslPrescriptionFilePath);
		assertEqual("b", model.saveAudioParameters().processing.rightDslPrescriptionFilePath);
	}

	TEST_F(PresenterTests, playCalibrationWithSpatializationPassesParametersToModel) {
		view.setSpatializationOn();
		view.setBrirFilePath("a");
		view.playCalibration();
		assertEqual("a", model.calibrationParameters().processing.brirFilePath);
	}

	TEST_F(PresenterTests, saveAudioWithSpatializationPassesParametersToModel) {
		view.setSpatializationOn();
		view.setBrirFilePath("a");
		view.processAudioForSaving();
		assertEqual("a", model.saveAudioParameters().processing.brirFilePath);
	}

	TEST_F(
		PresenterTests,
		playCalibrationUsingSpatialization
	) {
		view.setSpatializationOn();
		view.playCalibration();
		assertTrue(model.calibrationParameters().processing.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		playCalibrationNotUsingSpatialization
	) {
		view.setSpatializationOff();
		view.playCalibration();
		assertFalse(model.calibrationParameters().processing.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		saveAudioUsingSpatialization
	) {
		view.setSpatializationOn();
		view.processAudioForSaving();
		assertTrue(model.saveAudioParameters().processing.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		saveAudioNotUsingSpatialization
	) {
		view.setSpatializationOff();
		view.processAudioForSaving();
		assertFalse(model.saveAudioParameters().processing.usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		playCalibrationUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOn();
		view.playCalibration();
		assertTrue(model.calibrationParameters().processing.usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		playCalibrationNotUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOff();
		view.playCalibration();
		assertFalse(model.calibrationParameters().processing.usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		saveAudioUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOn();
		view.processAudioForSaving();
		assertTrue(model.saveAudioParameters().processing.usingHearingAidSimulation);
	}

	TEST_F(
		PresenterTests,
		saveAudioNotUsingHearingAidSimulation
	) {
		view.setHearingAidSimulationOff();
		view.processAudioForSaving();
		assertFalse(model.saveAudioParameters().processing.usingHearingAidSimulation);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidLevelShowsErrorMessage) {
		playCalibrationWithLevelShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, saveAudioWithInvalidLevelShowsErrorMessage) {
		saveAudioWithLevelShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidLevelDoesNotPlay) {
		setInvalidLevel();
		playCalibrationDoesNotPlay();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		playCalibrationWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, saveAudioWithInvalidAttackShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		saveAudioWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackDoesNotPlay) {
		view.setHearingAidSimulationOn();
		setInvalidAttackTime();
		playCalibrationDoesNotPlay();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackButNoHearingAidSimulationStillPlays) {
		view.setHearingAidSimulationOff();
		setInvalidAttackTime();
		playCalibrationPlays();
	}

	TEST_F(PresenterTests, saveAudioWithInvalidAttackButNoHearingAidSimulationStillSaves) {
		view.setHearingAidSimulationOff();
		setInvalidAttackTime();
		saveAudioSaves();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidReleaseShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		playCalibrationWithReleaseTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, saveAudioWithInvalidReleaseShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		saveAudioWithReleaseTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidReleaseDoesNotPlay) {
		view.setHearingAidSimulationOn();
		setInvalidReleaseTime();
		playCalibrationDoesNotPlay();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidReleaseButNoHearingAidSimulationStillPlays) {
		view.setHearingAidSimulationOff();
		setInvalidReleaseTime();
		playCalibrationPlays();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidChunkSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			playCalibrationWithChunkSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidChunkSizeDoesNotPlay) {
		view.setHearingAidSimulationOn();
		setInvalidChunkSize();
		playCalibrationDoesNotPlay();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidChunkSizeButNoHearingAidSimulationStillPlays) {
		view.setHearingAidSimulationOff();
		setInvalidChunkSize();
		playCalibrationPlays();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidWindowSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			playCalibrationWithWindowSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidWindowSizeDoesNotPlay) {
		view.setHearingAidSimulationOn();
		setInvalidWindowSize();
		playCalibrationDoesNotPlay();
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidWindowSizeButNoHearingAidSimulationStillPlays) {
		view.setHearingAidSimulationOff();
		setInvalidWindowSize();
		playCalibrationPlays();
	}

	TEST_F(PresenterTests, stopCalibrationStopsCalibration) {
		view.stopCalibration();
		assertTrue(model.calibrationStopped());
	}

	class PresenterWithFailingModel : public ::testing::Test {
	protected:
		FailingModel model;
		ViewStub view;
		Presenter presenter{ &model, &view };
	};

	TEST_F(PresenterWithFailingModel, confirmTestSetupShowsErrorMessage) {
		model.setErrorMessage("error.");
		view.confirmTestSetup();
		assertEqual("error.", view.errorMessage());
	}

	TEST_F(PresenterWithFailingModel, confirmTestSetupDoesNotShowTesterView) {
		view.confirmTestSetup();
		assertFalse(view.testerViewShown());
	}

	TEST_F(PresenterWithFailingModel, confirmTestSetupDoesNotHideSetupView) {
		view.confirmTestSetup();
		assertFalse(view.everyItemInTestSetupHidden());
	}

	TEST_F(PresenterWithFailingModel, playTrialShowsErrorMessage) {
		model.setErrorMessage("error.");
		view.playNextTrial();
		assertEqual("error.", view.errorMessage());
	}

	TEST_F(PresenterWithFailingModel, playCalibrationShowsErrorMessage) {
		model.setErrorMessage("error.");
		view.playCalibration();
		assertEqual("error.", view.errorMessage());
	}
}