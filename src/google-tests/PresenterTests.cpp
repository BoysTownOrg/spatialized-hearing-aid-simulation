#include "ModelStub.h"
#include "ViewStub.h"
#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>
#include <functional>

namespace {
	class UseCase {
	public:
        INTERFACE_OPERATIONS(UseCase)
		virtual void run(View::EventListener *) = 0;
	};

	class SignalProcessingUseCase : public UseCase {
	public:
		virtual const Model::SignalProcessing &processing(const ModelStub &) = 0;
	};

	class ConfirmingTestSetup : public SignalProcessingUseCase {
		void run(View::EventListener *listener) override {
			listener->confirmTestSetup();
		}

		const Model::SignalProcessing & processing(const ModelStub &m) override {
			return m.testing().processing;
		}
	};

	class PlayingCalibration : public SignalProcessingUseCase {
		void run(View::EventListener *listener) override {
			listener->playCalibration();
		}

		const Model::SignalProcessing & processing(const ModelStub &m) override {
			return m.calibration().processing;
		}
	};

	class SavingAudio : public SignalProcessingUseCase {
		void run(View::EventListener *listener) override {
			listener->saveAudio();
		}

		const Model::SignalProcessing & processing(const ModelStub &m) override {
			return m.savingAudio().processing;
		}
	};

	class BrowsingUseCase : public UseCase {
	public:
		virtual std::string &result(ViewStub &) = 0;
	};

	class BrowsingEnteredPathUseCase : public BrowsingUseCase {
	public:
		virtual std::string &entry(ViewStub &) = 0;
	};

	class BrowsingForFileUseCase : public BrowsingUseCase {
	public:
		virtual const std::vector<std::string> &filters(const ViewStub &) = 0;
	};

	class BrowsingForEnteredFilePathUseCase :
		public BrowsingEnteredPathUseCase,
		public BrowsingForFileUseCase {};

	const std::vector<std::string> &browseFiltersForOpeningFile(const ViewStub &view) noexcept {
		return view.browseFiltersForOpeningFile_;
	}

	const std::vector<std::string> &browseFiltersForSavingFile(const ViewStub &view) noexcept {
		return view.browseFiltersForSavingFile_;
	}

	std::string &browseForSavingFileResult(ViewStub &view) noexcept {
		return view.browseForSavingFileResult_;
	}

	std::string &browseForOpeningFileResult(ViewStub &view) noexcept {
		return view.browseForOpeningFileResult_;
	}

	std::string &browseForDirectoryResult(ViewStub &view) noexcept {
		return view.browseDirectory_;
	}
	
	class BrowsingForAudioFile : public BrowsingForEnteredFilePathUseCase {
		void run(View::EventListener *listener) override {
			listener->browseForAudioFile();
		}

		std::string &entry(ViewStub &view) override {
			return view.testSetup_.audioFilePath_;
		}

		std::string &result(ViewStub &view) override {
			return browseForOpeningFileResult(view);
		}

		const std::vector<std::string>& filters(const ViewStub &view) override {
			return browseFiltersForOpeningFile(view);
		}
	};

	class BrowsingForBrir : public BrowsingForEnteredFilePathUseCase {
		void run(View::EventListener *listener) override {
			listener->browseForBrir();
		}

		std::string &entry(ViewStub &view) override {
			return view.testSetup_.brirFilePath_;
		}

		std::string &result(ViewStub &view) override {
			return browseForOpeningFileResult(view);
		}

		const std::vector<std::string>& filters(const ViewStub &view) override {
			return browseFiltersForOpeningFile(view);
		}
	};

	class BrowsingForLeftDslPrescription : public BrowsingForEnteredFilePathUseCase {
		void run(View::EventListener *listener) override {
			listener->browseForLeftDslPrescription();
		}

		std::string &entry(ViewStub &view) override {
			return view.testSetup_.leftDslPrescriptionFilePath_;
		}

		std::string &result(ViewStub &view) override {
			return browseForOpeningFileResult(view);
		}

		const std::vector<std::string>& filters(const ViewStub &view) override {
			return browseFiltersForOpeningFile(view);
		}
	};

	class BrowsingForRightDslPrescription : public BrowsingForEnteredFilePathUseCase {
		void run(View::EventListener *listener) override {
			listener->browseForRightDslPrescription();
		}

		std::string &entry(ViewStub &view) override {
			return view.testSetup_.rightDslPrescriptionFilePath_;
		}

		std::string &result(ViewStub &view) override {
			return browseForOpeningFileResult(view);
		}

		const std::vector<std::string>& filters(const ViewStub &view) override {
			return browseFiltersForOpeningFile(view);
		}
	};

	class BrowsingForStimulusList : public BrowsingEnteredPathUseCase {
		void run(View::EventListener *listener) override {
			listener->browseForStimulusList();
		}

		std::string &entry(ViewStub &view) override {
			return view.testSetup_.stimulusList_;
		}

		std::string &result(ViewStub &view) override {
			return browseForDirectoryResult(view);
		}
	};

	class BrowsingForTestFile : public BrowsingForEnteredFilePathUseCase {
		void run(View::EventListener *listener) override {
			listener->browseForTestFile();
		}

		std::string &entry(ViewStub &view) override {
			return view.testSetup_.testFilePath_;
		}

		std::string &result(ViewStub &view) override {
			return browseForSavingFileResult(view);
		}

		const std::vector<std::string>& filters(const ViewStub &view) override {
			return browseFiltersForSavingFile(view);
		}
	};

	class BrowsingForSavingAudio : public BrowsingForFileUseCase {
		void run(View::EventListener *listener) override {
			listener->saveAudio();
		}

		std::string &result(ViewStub &view) override {
			return browseForSavingFileResult(view);
		}

		const std::vector<std::string>& filters(const ViewStub &view) override {
			return browseFiltersForSavingFile(view);
		}
	};

	void assertSpatializationUIHasBeenDeactivated(const ViewStub &view) {
		assertTrue(view.brirFilePathDeactivated_);
		assertTrue(view.browseForBrirButtonDeactivated_);
	}

	void assertSpatializationUIHasNotBeenActivated(const ViewStub &view) {
		assertFalse(view.brirFilePathActivated_);
		assertFalse(view.browseForBrirButtonActivated_);
	}

	void assertSpatializationUIHasBeenActivated(const ViewStub &view) {
		assertTrue(view.brirFilePathActivated_);
		assertTrue(view.browseForBrirButtonActivated_);
	}

	void assertSpatializationUIHasNotBeenDeactivated(const ViewStub &view) {
		assertFalse(view.brirFilePathDeactivated_);
		assertFalse(view.browseForBrirButtonDeactivated_);
	}

	void assertHearingAidUIHasBeenActivated(const ViewStub &view) {
		assertTrue(view.leftDslPrescriptionFilePathActivated_);
		assertTrue(view.rightDslPrescriptionFilePathActivated_);
		assertTrue(view.browseForLeftDslPrescriptionButtonActivated_);
		assertTrue(view.browseForRightDslPrescriptionButtonActivated_);
		assertTrue(view.chunkSizeActivated_);
		assertTrue(view.windowSizeActivated_);
		assertTrue(view.attack_msActivated_);
		assertTrue(view.release_msActivated_);
	}

	void assertHearingAidUIHasNotBeenDeactivated(const ViewStub &view) {
		assertFalse(view.leftDslPrescriptionFilePathDeactivated_);
		assertFalse(view.rightDslPrescriptionFilePathDeactivated_);
		assertFalse(view.browseForLeftDslPrescriptionButtonDeactivated_);
		assertFalse(view.browseForRightDslPrescriptionButtonDeactivated_);
		assertFalse(view.chunkSizeDeactivated_);
		assertFalse(view.windowSizeDeactivated_);
		assertFalse(view.attack_msDeactivated_);
		assertFalse(view.release_msDeactivated_);
	}

	void assertHearingAidUIHasNotBeenActivated(const ViewStub &view) {
		assertFalse(view.leftDslPrescriptionFilePathActivated_);
		assertFalse(view.rightDslPrescriptionFilePathActivated_);
		assertFalse(view.browseForLeftDslPrescriptionButtonActivated_);
		assertFalse(view.browseForRightDslPrescriptionButtonActivated_);
		assertFalse(view.chunkSizeActivated_);
		assertFalse(view.windowSizeActivated_);
		assertFalse(view.attack_msActivated_);
		assertFalse(view.release_msActivated_);
	}

	void assertHearingAidUIHasBeenDeactivated(const ViewStub &view) {
		assertTrue(view.leftDslPrescriptionFilePathDeactivated_);
		assertTrue(view.rightDslPrescriptionFilePathDeactivated_);
		assertTrue(view.browseForLeftDslPrescriptionButtonDeactivated_);
		assertTrue(view.browseForRightDslPrescriptionButtonDeactivated_);
		assertTrue(view.chunkSizeDeactivated_);
		assertTrue(view.windowSizeDeactivated_);
		assertTrue(view.attack_msDeactivated_);
		assertTrue(view.release_msDeactivated_);
	}

	class PresenterConstructionTests : public ::testing::Test {
	protected:
		ModelStub model;
		ViewStub view;

		void assertSpatializationUIHasOnlyBeenDeactivated() {
			assertSpatializationUIHasBeenDeactivated(view);
			assertSpatializationUIHasNotBeenActivated(view);
		}

		void assertSpatializationUIHasOnlyBeenActivated() {
			assertSpatializationUIHasBeenActivated(view);
			assertSpatializationUIHasNotBeenDeactivated(view);
		}

		void assertHearingAidUIHasOnlyBeenDeactivated() {
			assertHearingAidUIHasNotBeenActivated(view);
			assertHearingAidUIHasBeenDeactivated(view);
		}

		void assertHearingAidUIHasOnlyBeenActivated() {
			assertHearingAidUIHasBeenActivated(view);
			assertHearingAidUIHasNotBeenDeactivated(view);
		}

		Presenter construct() {
			return { &model, &view };
		}
	};

	TEST_F(PresenterConstructionTests, constructorPopulatesAudioDeviceMenu) {
		model.setAudioDeviceDescriptions({ "a", "b", "c" });
		auto presenter = construct();
		assertEqual({ "a", "b", "c" }, view.audioDeviceMenuItems());
	}

	TEST_F(PresenterConstructionTests, constructorActivatesSpatializationUIWhenInitiallyOn) {
		view.setSpatializationOn();
		auto presenter = construct();
		assertSpatializationUIHasOnlyBeenActivated();
	}

	TEST_F(PresenterConstructionTests, constructorDeactivatesSpatializationUIWhenInitiallyOff) {
		view.setSpatializationOff();
		auto presenter = construct();
		assertSpatializationUIHasOnlyBeenDeactivated();
	}

	TEST_F(PresenterConstructionTests, constructorActivatesHearingAidSimulationUIWhenInitiallyOn) {
		view.setHearingAidSimulationOn();
		auto presenter = construct();
		assertHearingAidUIHasOnlyBeenActivated();
	}

	TEST_F(PresenterConstructionTests, constructorDeactivatesHearingAidSimulationUIWhenInitiallyOff) {
		view.setHearingAidSimulationOff();
		auto presenter = construct();
		assertHearingAidUIHasOnlyBeenDeactivated();
	}

	class PresenterTests : public ::testing::Test {
	protected:
		ModelStub model;
		ViewStub view;
		Presenter presenter{ &model, &view };
		ConfirmingTestSetup confirmingTestSetup{};
		PlayingCalibration playingCalibration{};
		SavingAudio savingAudio{};
		BrowsingForAudioFile browsingForAudioFile{};
		BrowsingForBrir browsingForBrir{};
		BrowsingForLeftDslPrescription browsingForLeftDslPrescription{};
		BrowsingForRightDslPrescription browsingForRightDslPrescription{};
		BrowsingForStimulusList browsingForStimulusList{};
		BrowsingForTestFile browsingForTestFile{};
		BrowsingForSavingAudio browsingForSavingAudio{};

		void runUseCase(UseCase *useCase) {
			useCase->run(&presenter);
		}

		void assertSpatializationUIHasOnlyBeenDeactivated() {
			assertSpatializationUIHasBeenDeactivated(view);
			assertSpatializationUIHasNotBeenActivated(view);
		}

		void assertSpatializationUIHasOnlyBeenActivated() {
			assertSpatializationUIHasBeenActivated(view);
			assertSpatializationUIHasNotBeenDeactivated(view);
		}

		void assertHearingAidUIHasOnlyBeenDeactivated() {
			assertHearingAidUIHasNotBeenActivated(view);
			assertHearingAidUIHasBeenDeactivated(view);
		}

		void assertHearingAidUIHasOnlyBeenActivated() {
			assertHearingAidUIHasBeenActivated(view);
			assertHearingAidUIHasNotBeenDeactivated(view);
		}

		void assertCancellingBrowseDoesNotChangePath(BrowsingEnteredPathUseCase *useCase) {
			useCase->entry(view) = "a";
			view.setBrowseCancelled();
			runUseCase(useCase);
			assertEqual("a", useCase->entry(view));
		}

		void assertBrowseResultPassedToEntry(BrowsingEnteredPathUseCase *useCase) {
			useCase->result(view) = "a";
			runUseCase(useCase);
			assertEqual("a", useCase->entry(view));
		}

		void assertBrowsingFilters(BrowsingForFileUseCase *useCase, std::vector<std::string> expected) {
			runUseCase(useCase);
			assertEqual(std::move(expected), useCase->filters(view));
		}

		void setInvalidChunkSize() {
			view.testSetup_.setChunkSize("a");
		}

		void setInvalidWindowSize() {
			view.testSetup_.setWindowSize("a");
		}

		void setInvalidReleaseTime() {
			view.testSetup_.setRelease_ms("a");
		}

		void setInvalidAttackTime() {
			view.testSetup_.setAttack_ms("a");
		}

		void setInvalidLevel() {
			view.testSetup_.setLevel_dB_Spl("a");
		}

		void assertTesterViewHidden() {
			assertTrue(view.everyItemInTesterViewHidden());
		}

		void assertTestSetupViewShown() {
			assertTrue(view.testSetup_.shown_);
		}

		void assertTestSetupViewNotHidden() {
			assertFalse(view.testSetup_.hidden_);
		}

		void assertTestSetupViewHidden() {
			assertTrue(view.testSetup_.hidden_);
		}

		void assertTestSetupViewNotShown() {
			assertTrue(view.notASingleElementInTesterViewShown());
		}

		void assertTesterViewShown() {
			assertTrue(view.everyItemInTesterViewShown());
		}

		void confirmTestSetupDoesNotHideSetupView() {
			const auto assertTestSetupViewNotHidden = &PresenterTests::assertTestSetupViewNotHidden;
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
			const auto assertTestSetupViewHidden = &PresenterTests::assertTestSetupViewHidden;
			confirmTestSetupThen(assertTestSetupViewHidden);
		}

		void confirmTestSetupDoesNotShowTesterView() {
			const auto assertTestSetupViewNotShown = &PresenterTests::assertTestSetupViewNotShown;
			confirmTestSetupThen(assertTestSetupViewNotShown);
		}

		void confirmTestSetupShowsTesterView() {
			const auto assertTesterViewShown = &PresenterTests::assertTesterViewShown;
			confirmTestSetupThen(assertTesterViewShown);
		}

		void confirmTestSetupDoesNotPrepareTest() {
			const auto assertTestHasNotBeenPrepared = &PresenterTests::assertTestHasNotBeenPrepared;
			confirmTestSetupThen(assertTestHasNotBeenPrepared);
		}

		void assertTestHasNotBeenPrepared() {
			assertFalse(model.testPrepared());
		}

		void confirmTestSetupWithChunkSizeShowsErrorMessage(std::string s) {
			view.testSetup_.setChunkSize(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid chunk size.");
		}

		void confirmTestSetupShowsErrorMessage(std::string s) {
			const auto assertErrorMessageEquals = &PresenterTests::assertErrorMessageEquals;
			confirmTestSetupThen(assertErrorMessageEquals, std::move(s));
		}

		void assertErrorMessageEquals(std::string s) {
			assertEqual(std::move(s), view.errorMessage());
		}

		void confirmTestSetupWithWindowSizeShowsErrorMessage(std::string s) {
			view.testSetup_.setWindowSize(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid window size.");
		}

		void confirmTestSetupWithAttackTimeShowsErrorMessage(std::string s) {
			view.testSetup_.setAttack_ms(s);
			confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}

		void confirmTestSetupWithReleaseTimeShowsErrorMessage(std::string s) {
			view.testSetup_.setRelease_ms(s);
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
			view.saveAudio();
			assertTrue(model.audioSaved());
		}
		
		void playCalibrationWithAttackTimeShowsErrorMessage(std::string s) {
			view.testSetup_.setAttack_ms(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}

		void saveAudioWithAttackTimeShowsErrorMessage(std::string s) {
			view.testSetup_.setAttack_ms(s);
			saveAudioShowsErrorMessage("'" + s + "' is not a valid attack time.");
		}
		
		void playCalibrationWithReleaseTimeShowsErrorMessage(std::string s) {
			view.testSetup_.setRelease_ms(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid release time.");
		}
		
		void saveAudioWithReleaseTimeShowsErrorMessage(std::string s) {
			view.testSetup_.setRelease_ms(s);
			saveAudioShowsErrorMessage("'" + s + "' is not a valid release time.");
		}
		
		void playCalibrationWithChunkSizeShowsErrorMessage(std::string s) {
			view.testSetup_.setChunkSize(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid chunk size.");
		}

		void playCalibrationWithWindowSizeShowsErrorMessage(std::string s) {
			view.testSetup_.setWindowSize(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid window size.");
		}

		void playCalibrationWithLevelShowsErrorMessage(std::string s) {
			view.testSetup_.setLevel_dB_Spl(s);
			playCalibrationShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void saveAudioWithLevelShowsErrorMessage(std::string s) {
			view.testSetup_.setLevel_dB_Spl(s);
			saveAudioShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void playCalibrationShowsErrorMessage(std::string s) {
			view.playCalibration();
			assertErrorMessageEquals(std::move(s));
		}

		void saveAudioShowsErrorMessage(std::string s) {
			view.saveAudio();
			assertErrorMessageEquals(std::move(s));
		}
		
		void playTrialWithLevelShowsErrorMessage(std::string s) {
			view.testSetup_.setLevel_dB_Spl(s);
			playTrialShowsErrorMessage("'" + s + "' is not a valid level.");
		}

		void playTrialShowsErrorMessage(std::string s) {
			view.playNextTrial();
			assertErrorMessageEquals(std::move(s));
		}

		void assertHearingAidSimulationMatchesViewFollowingRequest(SignalProcessingUseCase *useCase) {
			view.setHearingAidSimulationOn();
			view.testSetup_.setAttack_ms("1.1");
			view.testSetup_.setRelease_ms("2.2");
			view.testSetup_.setChunkSize("3");
			view.testSetup_.setWindowSize("4");
			view.testSetup_.setLeftDslPrescriptionFilePath("a");
			view.testSetup_.setRightDslPrescriptionFilePath("b");
			runUseCase(useCase);
			assertEqual(1.1, useCase->processing(model).attack_ms);
			assertEqual(2.2, useCase->processing(model).release_ms);
			assertEqual(3, useCase->processing(model).chunkSize);
			assertEqual(4, useCase->processing(model).windowSize);
			assertEqual("a", useCase->processing(model).leftDslPrescriptionFilePath);
			assertEqual("b", useCase->processing(model).rightDslPrescriptionFilePath);
		}

		void assertSpatializationMatchesViewFollowingRequest(SignalProcessingUseCase *useCase) {
			view.setSpatializationOn();
			view.testSetup_.setBrirFilePath("a");
			runUseCase(useCase);
			assertEqual("a", useCase->processing(model).brirFilePath);
		}

		void assertUsingSpatializationFollowingRequest(SignalProcessingUseCase *useCase) {
			view.setSpatializationOn();
			runUseCase(useCase);
			assertTrue(useCase->processing(model).usingSpatialization);
		}

		void assertNotUsingSpatializationFollowingRequest(SignalProcessingUseCase *useCase) {
			view.setSpatializationOff();
			runUseCase(useCase);
			assertFalse(useCase->processing(model).usingSpatialization);
		}

		void assertUsingHearingAidSimulationFollowingRequest(SignalProcessingUseCase *useCase) {	
			view.setHearingAidSimulationOn();
			runUseCase(useCase);
			assertTrue(useCase->processing(model).usingHearingAidSimulation);
		}

		void assertNotUsingHearingAidSimulationFollowingRequest(SignalProcessingUseCase *useCase) {	
			view.setHearingAidSimulationOff();
			runUseCase(useCase);
			assertFalse(useCase->processing(model).usingHearingAidSimulation);
		}
	};

	TEST_F(PresenterTests, constructorSubscribesToViewEvents) {
		EXPECT_EQ(&presenter, view.listener());
	}

	TEST_F(PresenterTests, runRunsEventLoop) {
		presenter.run();
		assertTrue(view.runningEventLoop());
	}

	TEST_F(PresenterTests, constructorShowsTestSetupView) {
		assertTestSetupViewShown();
	}

	TEST_F(PresenterTests, cancellingBrowseForAudioFileDoesNotChangeAudioFilePath) {
		assertCancellingBrowseDoesNotChangePath(&browsingForAudioFile);
	}

	TEST_F(PresenterTests, cancellingBrowseForTestFileDoesNotChangeTestFilePath) {
		assertCancellingBrowseDoesNotChangePath(&browsingForTestFile);
	}

	TEST_F(PresenterTests, cancellingBrowseForDslPrescriptionDoesNotChangeDslPrescriptionFilePath) {
		assertCancellingBrowseDoesNotChangePath(&browsingForLeftDslPrescription);
		assertCancellingBrowseDoesNotChangePath(&browsingForRightDslPrescription);
	}

	TEST_F(PresenterTests, cancellingBrowseForStimulusListNotChangeStimulusList) {
		assertCancellingBrowseDoesNotChangePath(&browsingForStimulusList);
	}

	TEST_F(PresenterTests, cancellingBrowseForBrirDoesNotChangeBrirFilePath) {
		assertCancellingBrowseDoesNotChangePath(&browsingForBrir);
	}

	TEST_F(PresenterTests, browseForTestFileFiltersTextFiles) {
		assertBrowsingFilters(&browsingForTestFile, { "*.txt" });
	}

	TEST_F(PresenterTests, browseForAudioFileFiltersAudioFiles) {
		assertBrowsingFilters(&browsingForAudioFile, { "*.wav" });
	}

	TEST_F(PresenterTests, browseForBrirFiltersWavFiles) {
		assertBrowsingFilters(&browsingForBrir, { "*.wav" });
	}

	TEST_F(PresenterTests, browseForPrescriptionsFiltersJsonFiles) {
		assertBrowsingFilters(&browsingForLeftDslPrescription, { "*.json" });
		assertBrowsingFilters(&browsingForRightDslPrescription, { "*.json" });
	}

	TEST_F(PresenterTests, saveAudioFiltersWavFiles) {
		assertBrowsingFilters(&browsingForSavingAudio, { "*.wav" });
	}

	TEST_F(PresenterTests, browseForTestFileUpdatesTestFilePath) {
		assertBrowseResultPassedToEntry(&browsingForTestFile);
	}

	TEST_F(PresenterTests, browseForAudioFileUpdatesAudioFilePath) {
		assertBrowseResultPassedToEntry(&browsingForAudioFile);
	}

	TEST_F(PresenterTests, browseForDslPrescriptionUpdatesDslPrescriptionFilePath) {
		assertBrowseResultPassedToEntry(&browsingForLeftDslPrescription);
		assertBrowseResultPassedToEntry(&browsingForRightDslPrescription);
	}

	TEST_F(PresenterTests, browseForStimulusListUpdatesStimulusList) {
		assertBrowseResultPassedToEntry(&browsingForStimulusList);
	}

	TEST_F(PresenterTests, browseForBrirUpdatesBrirFilePath) {
		assertBrowseResultPassedToEntry(&browsingForBrir);
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

	TEST_F(PresenterTests, confirmTestSetupPassesParametersToModel) {
		view.testSetup_.setStimulusList("a");
		view.testSetup_.setTestFilePath("b");
		view.setSubjectId("c");
		view.setTesterId("d");
		view.confirmTestSetup();
		assertEqual("a", model.testing().audioDirectory);
		assertEqual("b", model.testing().testFilePath);
		assertEqual("c", model.testing().subjectId);
		assertEqual("d", model.testing().testerId);
	}

	TEST_F(PresenterTests, playTrialPassesParametersToModel) {
		view.setAudioDevice("a");
		view.testSetup_.setLevel_dB_Spl("1.1");
		view.playNextTrial();
		assertEqual("a", model.trial().audioDevice);
		assertEqual(1.1, model.trial().level_dB_Spl);
	}

	TEST_F(PresenterTests, playCalibrationPassesCalibrationParametersToModel) {
		view.setAudioDevice("a");
		view.testSetup_.setAudioFilePath("b");
		view.testSetup_.setLevel_dB_Spl("1.1");
		view.playCalibration();
		assertEqual("a", model.calibration().audioDevice);
		assertEqual("b", model.calibration().audioFilePath);
		assertEqual(1.1, model.calibration().level_dB_Spl);
	}

	TEST_F(PresenterTests, saveAudioPassesParametersToModel) {
		view.testSetup_.setAudioFilePath("a");
		view.testSetup_.setLevel_dB_Spl("1.1");
		view.saveAudio();
		assertEqual("a", model.savingAudio().inputAudioFilePath);
		assertEqual(1.1, model.savingAudio().level_dB_Spl);
	}

	TEST_F(PresenterTests, saveAudioSavesAfterProcessing) {
		view.saveAudio();
		assertEqual("processAudioForSaving saveAudio ", model.saveAudioLog());
	}

	TEST_F(
		PresenterTests,
		saveAudioPassesOutputAudioFileToModel
	) {
		view.setBrowseForSavingFileResult("a");
		view.saveAudio();
		assertEqual("a", model.savedAudioFilePath());
	}

	TEST_F(
		PresenterTests,
		cancellingSaveAudioDoesNotSaveAudio
	) {
		view.cancelWhenBrowsingForSavingFile();
		view.saveAudio();
		assertFalse(model.audioSaved());
	}

	TEST_F(PresenterTests, confirmTestSetupPassesHearingAidParametersToModel) {
		assertHearingAidSimulationMatchesViewFollowingRequest(&confirmingTestSetup);
	}

	TEST_F(PresenterTests, playCalibrationWithHearingAidSimulationPassesParametersToModel) {
		assertHearingAidSimulationMatchesViewFollowingRequest(&playingCalibration);
	}

	TEST_F(PresenterTests, saveAudioWithHearingAidSimulationPassesParametersToModel) {
		assertHearingAidSimulationMatchesViewFollowingRequest(&savingAudio);
	}

	TEST_F(PresenterTests, confirmTestSetupPassesSpatializationParametersToModel) {
		assertSpatializationMatchesViewFollowingRequest(&confirmingTestSetup);
	}

	TEST_F(PresenterTests, playCalibrationWithSpatializationPassesParametersToModel) {
		assertSpatializationMatchesViewFollowingRequest(&playingCalibration);
	}

	TEST_F(PresenterTests, saveAudioWithSpatializationPassesParametersToModel) {
		assertSpatializationMatchesViewFollowingRequest(&savingAudio);
	}

	TEST_F(PresenterTests, confirmTestSetupUsingSpatialization) {
		assertUsingSpatializationFollowingRequest(&confirmingTestSetup);
	}

	TEST_F(PresenterTests, playCalibrationUsingSpatialization) {
		assertUsingSpatializationFollowingRequest(&playingCalibration);
	}

	TEST_F(PresenterTests, saveAudioUsingSpatialization) {
		assertUsingSpatializationFollowingRequest(&savingAudio);
	}

	TEST_F(PresenterTests, confirmTestSetupNotUsingSpatialization) {
		assertNotUsingSpatializationFollowingRequest(&confirmingTestSetup);
	}

	TEST_F(PresenterTests, playCalibrationNotUsingSpatialization) {
		assertNotUsingSpatializationFollowingRequest(&playingCalibration);
	}

	TEST_F(PresenterTests, saveAudioNotUsingSpatialization) {
		assertNotUsingSpatializationFollowingRequest(&savingAudio);
	}

	TEST_F(PresenterTests, confirmTestSetupUsingHearingAidSimulation) {
		assertUsingHearingAidSimulationFollowingRequest(&confirmingTestSetup);
	}

	TEST_F(PresenterTests, playCalibrationUsingHearingAidSimulation) {
		assertUsingHearingAidSimulationFollowingRequest(&playingCalibration);
	}

	TEST_F(PresenterTests, saveAudioUsingHearingAidSimulation) {
		assertUsingHearingAidSimulationFollowingRequest(&savingAudio);
	}

	TEST_F(PresenterTests, confirmTestSetupNotUsingHearingAidSimulation) {
		assertNotUsingHearingAidSimulationFollowingRequest(&confirmingTestSetup);
	}

	TEST_F(PresenterTests, playCalibrationNotUsingHearingAidSimulation) {
		assertNotUsingHearingAidSimulationFollowingRequest(&playingCalibration);
	}

	TEST_F(PresenterTests, saveAudioNotUsingHearingAidSimulation) {
		assertNotUsingHearingAidSimulationFollowingRequest(&savingAudio);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidLevelDoesNotPlay) {
		setInvalidLevel();
		playCalibrationDoesNotPlay();
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

	TEST_F(PresenterTests, playTrialWithInvalidLevelShowsErrorMessage) {
		playTrialWithLevelShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidLevelShowsErrorMessage) {
		playCalibrationWithLevelShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, saveAudioWithInvalidLevelShowsErrorMessage) {
		saveAudioWithLevelShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			confirmTestSetupWithChunkSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidChunkSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			playCalibrationWithChunkSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			confirmTestSetupWithWindowSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidWindowSizeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		for (auto s : { "a", "0.1", "-1" })
			playCalibrationWithWindowSizeShowsErrorMessage(s);
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		confirmTestSetupWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidAttackShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		playCalibrationWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, saveAudioWithInvalidAttackShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		saveAudioWithAttackTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		confirmTestSetupWithReleaseTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, playCalibrationWithInvalidReleaseShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		playCalibrationWithReleaseTimeShowsErrorMessage("a");
	}

	TEST_F(PresenterTests, saveAudioWithInvalidReleaseShowsErrorMessage) {
		view.setHearingAidSimulationOn();
		saveAudioWithReleaseTimeShowsErrorMessage("a");
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
		assertFalse(view.testSetup_.hidden_);
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
