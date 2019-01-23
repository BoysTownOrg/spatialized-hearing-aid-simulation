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

		void confirmTestSetupDoesNotShowTesterView() {
			view.confirmTestSetup();
			EXPECT_FALSE(view.testerViewShown());
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

	TEST_F(PresenterTests, constructorPopulatesChunkAndWindowSizesWithPreferredProcessingSizes) {
		model.setPreferredProcessingSizes({ 1, 2, 3 });
		Presenter{ &model, &view };
		assertEqual(
			{ "1", "2", "3" },
			view.chunkSizeItems()
		);
		assertEqual(
			{ "1", "2", "3" },
			view.windowSizeItems()
		);
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

	TEST_F(PresenterTests, confirmTestSetupHidesTestSetupView) {
		view.confirmTestSetup();
		EXPECT_TRUE(view.testSetupHidden());
	}

	TEST_F(PresenterTests, confirmTestSetupShowsTesterView) {
		view.confirmTestSetup();
		EXPECT_TRUE(view.testerViewShown());
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotHideSetupView) {
		setInvalidChunkSize();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotHideSetupView) {
		setInvalidWindowSize();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotHideSetupView) {
		setInvalidReleaseTime();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotHideSetupView) {
		setInvalidAttackTime();
		confirmTestSetupDoesNotHideSetupView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotShowTesterView) {
		setInvalidChunkSize();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotShowTesterView) {
		setInvalidWindowSize();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotShowTesterView) {
		setInvalidReleaseTime();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotShowTesterView) {
		setInvalidAttackTime();
		confirmTestSetupDoesNotShowTesterView();
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeShowsErrorMessage) {
		for (auto s : { "a", "0.1", "-1" })
			confirmTestSetupWithChunkSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeShowsErrorMessage) {
		for (auto s : { "a", "0.1", "-1" })
			confirmTestSetupWithWindowSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeShowsErrorMessage) {
		confirmTestSetupWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeShowsErrorMessage) {
		confirmTestSetupWithReleaseTimeShowsErrorMessage("b");
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupPassesParametersToModel
	) {
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
		EXPECT_EQ(2.2, model.testParameters().attack_ms);
		EXPECT_EQ(3.3, model.testParameters().release_ms);
		EXPECT_EQ(4, model.testParameters().windowSize);
		EXPECT_EQ(5, model.testParameters().chunkSize);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupUsingSpatialization
	) {
		view.setSpatializationOn();
		view.confirmTestSetup();
		EXPECT_TRUE(model.testParameters().usingSpatialization);
	}

	TEST_F(
		PresenterTests,
		confirmTestSetupNotUsingSpatialization
	) {
		view.setSpatializationOff();
		view.confirmTestSetup();
		EXPECT_FALSE(model.testParameters().usingSpatialization);
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