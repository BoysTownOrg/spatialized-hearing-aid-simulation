#pragma once

#include <presentation/View.h>

class ViewStub : public View {
public:
	std::vector<std::string> browseFiltersForOpeningFile_{};
	std::vector<std::string> browseFiltersForSavingFile_{};
	std::vector<std::string> audioDeviceMenuItems_{};
	std::vector<std::string> chunkSizeItems_{};
	std::vector<std::string> windowSizeItems_{};
	std::string testFilePath_{};
	std::string stimulusList_{};
	std::string leftDslPrescriptionFilePath_{};
	std::string rightDslPrescriptionFilePath_{};
	std::string brirFilePath_{};
	std::string audioFilePath_{};
	std::string subjectId_{};
	std::string testerId_{};
	std::string audioDevice_{};
	std::string level_dB_Spl_{ "0" };
	std::string attack_ms_{ "0" };
	std::string release_ms_{ "0" };
	std::string windowSize_{ "0" };
	std::string chunkSize_{ "0" };
	std::string errorMessage_{};
	std::string browseForOpeningFileResult_{};
	std::string browseForSavingFileResult_{};
	std::string browseDirectory_{};
	EventListener *listener_{};
	bool runningEventLoop_{};
	bool browseCancelled_{};
	bool testerViewShown_{};
	bool testerViewHidden_{};
	bool usingSpatialization_{};
	bool brirFilePathDeactivated_{};
	bool browseForBrirButtonDeactivated_{};
	bool brirFilePathActivated_{};
	bool browseForBrirButtonActivated_{};
	bool usingHearingAidSimulation_{};
	bool browseForLeftDslPrescriptionButtonActivated_{};
	bool browseForRightDslPrescriptionButtonActivated_{};
	bool leftDslPrescriptionFilePathActivated_{};
	bool rightDslPrescriptionFilePathActivated_{};
	bool leftDslPrescriptionFilePathDeactivated_{};
	bool rightDslPrescriptionFilePathDeactivated_{};
	bool browseForLeftDslPrescriptionButtonDeactivated_{};
	bool browseForRightDslPrescriptionButtonDeactivated_{};
	bool chunkSizeActivated_{};
	bool windowSizeActivated_{};
	bool attack_msActivated_{};
	bool release_msActivated_{};
	bool chunkSizeDeactivated_{};
	bool windowSizeDeactivated_{};
	bool attack_msDeactivated_{};
	bool release_msDeactivated_{};
	bool subjectIdHidden_{};
	bool testerIdHidden_{};
	bool simulusListHidden_{};
	bool testFilePathHidden_{};
	bool confirmButtonHidden_{};
	bool brirFilePathHidden_{};
	bool browseForBrirButtonHidden_{};
	bool usingSpatializationCheckBoxHidden_{};
	bool leftDslPrescriptionFilePathHidden_{};
	bool browseForLeftDslPrescriptionButtonHidden_{};
	bool rightDslPrescriptionFilePathHidden_{};
	bool browseForRightDslPrescriptionButtonHidden_{};
	bool attack_msHidden_{};
	bool release_msHidden_{};
	bool chunkSizeHidden_{};
	bool windowSizeHidden_{};
	bool usingHearingAidSimulationCheckBoxHidden_{};
	bool audioFilePathHidden_{};
	bool playButtonHidden_{};
	bool saveButtonHidden_{};
	bool stopButtonHidden_{};
	bool level_dB_SplHidden_{};
	bool subjectIdShown_{};
	bool testerIdShown_{};
	bool simulusListShown_{};
	bool testFilePathShown_{};
	bool confirmButtonShown_{};
	bool brirFilePathShown_{};
	bool browseForBrirButtonShown_{};
	bool usingSpatializationCheckBoxShown_{};
	bool leftDslPrescriptionFilePathShown_{};
	bool browseForLeftDslPrescriptionButtonShown_{};
	bool rightDslPrescriptionFilePathShown_{};
	bool browseForRightDslPrescriptionButtonShown_{};
	bool attack_msShown_{};
	bool release_msShown_{};
	bool chunkSizeShown_{};
	bool windowSizeShown_{};
	bool usingHearingAidSimulationCheckBoxShown_{};
	bool audioFilePathShown_{};
	bool playButtonShown_{};
	bool saveButtonShown_{};
	bool stopButtonShown_{};
	bool level_dB_SplShown_{};
	bool playNextTrialButtonShown_{};
	bool playNextTrialButtonHidden_{};
	bool browseForAudioFileButtonHidden_{};
	bool browseForStimulusListButtonHidden_{};
	bool browseForAudioFileButtonShown_{};
	bool browseForStimulusListButtonShown_{};
	bool browseForTestFileButtonHidden_{};
	bool browseForTestFileButtonShown_{};
	bool cancelBrowsingForSavingFile_{};
	bool cancelOpeningForSavingFile_{};

	
	void showBrowseForTestFileButton() override {
		browseForTestFileButtonShown_ = true;
	}

	void hideBrowseForTestFileButton() override {
		browseForTestFileButtonHidden_ = true;
	}

	void browseForAudioFile() {
		listener_->browseForAudioFile();
	}

	void showBrowseForAudioFileButton() override {
		browseForAudioFileButtonShown_ = true;
	}

	void showBrowseForStimulusListButton() override {
		browseForStimulusListButtonShown_ = true;
	}

	void hideBrowseForStimulusListButton() override {
		browseForStimulusListButtonHidden_ = true;
	}

	void hideBrowseForAudioFileButton() override {
		browseForAudioFileButtonHidden_ = true;
	}

	void hidePlayNextTrialButton() override {
		playNextTrialButtonHidden_ = true;
	}

	void showPlayNextTrialButton() override {
		playNextTrialButtonShown_ = true;
	}

	bool everyItemInTesterViewHidden() noexcept {
		return
			playNextTrialButtonHidden_;
	}

	bool everyItemInTesterViewShown() noexcept {
		return
			playNextTrialButtonShown_;
	}

	bool notASingleElementInTesterViewShown() noexcept {
		return !(
			playNextTrialButtonShown_
		);
	}

	bool everyItemInTestSetupShown() noexcept {
		return
			subjectIdShown_ &&
			testerIdShown_ &&
			simulusListShown_ &&
			testFilePathShown_ &&
			browseForTestFileButtonShown_ &&
			confirmButtonShown_ &&
			brirFilePathShown_ &&
			browseForBrirButtonShown_ &&
			usingSpatializationCheckBoxShown_ &&
			leftDslPrescriptionFilePathShown_ &&
			browseForLeftDslPrescriptionButtonShown_ &&
			rightDslPrescriptionFilePathShown_ &&
			browseForRightDslPrescriptionButtonShown_ &&
			attack_msShown_ &&
			release_msShown_ &&
			chunkSizeShown_ &&
			windowSizeShown_ &&
			usingHearingAidSimulationCheckBoxShown_ &&
			audioFilePathShown_ &&
			playButtonShown_ &&
			stopButtonShown_ &&
			saveButtonShown_ &&
			level_dB_SplShown_ &&
			browseForAudioFileButtonShown_ &&
			browseForStimulusListButtonShown_;
	}

	bool everyItemInTestSetupHidden() noexcept {
		return
			subjectIdHidden_ &&
			testerIdHidden_ &&
			simulusListHidden_ &&
			testFilePathHidden_ &&
			browseForTestFileButtonHidden_ &&
			confirmButtonHidden_ &&
			brirFilePathHidden_ &&
			browseForBrirButtonHidden_ &&
			usingSpatializationCheckBoxHidden_ &&
			leftDslPrescriptionFilePathHidden_ &&
			browseForLeftDslPrescriptionButtonHidden_ &&
			rightDslPrescriptionFilePathHidden_ &&
			browseForRightDslPrescriptionButtonHidden_ &&
			attack_msHidden_ &&
			release_msHidden_ &&
			chunkSizeHidden_ &&
			windowSizeHidden_ &&
			usingHearingAidSimulationCheckBoxHidden_ &&
			audioFilePathHidden_ &&
			playButtonHidden_ &&
			stopButtonHidden_ &&
			saveButtonHidden_ &&
			level_dB_SplHidden_ &&
			browseForAudioFileButtonHidden_ &&
			browseForStimulusListButtonHidden_;
	}

	bool notASingleElementInTestSetupHidden() noexcept {
		return !(
			subjectIdHidden_ ||
			testerIdHidden_ ||
			simulusListHidden_ ||
			testFilePathHidden_ ||
			browseForTestFileButtonHidden_ ||
			confirmButtonHidden_ ||
			brirFilePathHidden_ ||
			browseForBrirButtonHidden_ ||
			usingSpatializationCheckBoxHidden_ ||
			leftDslPrescriptionFilePathHidden_ ||
			browseForLeftDslPrescriptionButtonHidden_ ||
			rightDslPrescriptionFilePathHidden_ ||
			browseForRightDslPrescriptionButtonHidden_ ||
			attack_msHidden_ ||
			release_msHidden_ ||
			chunkSizeHidden_ ||
			windowSizeHidden_ ||
			usingHearingAidSimulationCheckBoxHidden_ ||
			audioFilePathHidden_ ||
			playButtonHidden_ ||
			stopButtonHidden_ ||
			saveButtonHidden_ ||
			level_dB_SplHidden_ ||
			browseForAudioFileButtonHidden_ ||
			browseForStimulusListButtonHidden_
		);
	}

	void hideSubjectId() override {
		subjectIdHidden_ = true;
	}

	void hideTesterId() override {
		testerIdHidden_ = true;
	}
	
	void hideStimulusList() override {
		simulusListHidden_ = true;
	}
	
	void hideTestFilePath() override {
		testFilePathHidden_ = true;
	}
	
	void hideConfirmButton() override {
		confirmButtonHidden_ = true;
	}
	
	void hideBrirFilePath() override {
		brirFilePathHidden_ = true;
	}
	
	void hideBrowseForBrirButton() override {
		browseForBrirButtonHidden_ = true;
	}
	
	void hideUsingSpatializationCheckBox() override {
		usingSpatializationCheckBoxHidden_ = true;
	}
	
	void hideLeftDslPrescriptionFilePath() override {
		leftDslPrescriptionFilePathHidden_ = true;
	}
	
	void hideBrowseForLeftDslPrescriptionButton() override {
		browseForLeftDslPrescriptionButtonHidden_ = true;
	}
	
	void hideRightDslPrescriptionFilePath() override {
		rightDslPrescriptionFilePathHidden_ = true;
	}
	
	void hideBrowseForRightDslPrescriptionButton() override {
		browseForRightDslPrescriptionButtonHidden_ = true;
	}
	
	void hideAttack_ms() override {
		attack_msHidden_ = true;
	}
	
	void hideRelease_ms() override {
		release_msHidden_ = true;
	}
	
	void hideChunkSize() override {
		chunkSizeHidden_ = true;
	}
	
	void hideWindowSize() override {
		windowSizeHidden_ = true;
	}
	
	void hideUsingHearingAidSimulationCheckBox() override {
		usingHearingAidSimulationCheckBoxHidden_ = true;
	}
	
	void hideAudioFilePath() override {
		audioFilePathHidden_ = true;
	}
	
	void hidePlayButton() override {
		playButtonHidden_ = true;
	}
	
	void hideStopButton() override {
		stopButtonHidden_ = true;
	}
	
	void hideSaveButton() override {
		saveButtonHidden_ = true;
	}
	
	void hideLevel_dB_Spl() override {
		level_dB_SplHidden_ = true;
	}

	void showSubjectId() override {
		subjectIdShown_ = true;
	}

	void showTesterId() override {
		testerIdShown_ = true;
	}
	
	void showStimulusList() override {
		simulusListShown_ = true;
	}
	
	void showTestFilePath() override {
		testFilePathShown_ = true;
	}
	
	void showConfirmButton() override {
		confirmButtonShown_ = true;
	}
	
	void showBrirFilePath() override {
		brirFilePathShown_ = true;
	}
	
	void showBrowseForBrirButton() override {
		browseForBrirButtonShown_ = true;
	}
	
	void showUsingSpatializationCheckBox() override {
		usingSpatializationCheckBoxShown_ = true;
	}
	
	void showLeftDslPrescriptionFilePath() override {
		leftDslPrescriptionFilePathShown_ = true;
	}
	
	void showBrowseForLeftDslPrescriptionButton() override {
		browseForLeftDslPrescriptionButtonShown_ = true;
	}
	
	void showRightDslPrescriptionFilePath() override {
		rightDslPrescriptionFilePathShown_ = true;
	}
	
	void showBrowseForRightDslPrescriptionButton() override {
		browseForRightDslPrescriptionButtonShown_ = true;
	}
	
	void showAttack_ms() override {
		attack_msShown_ = true;
	}
	
	void showRelease_ms() override {
		release_msShown_ = true;
	}
	
	void showChunkSize() override {
		chunkSizeShown_ = true;
	}
	
	void showWindowSize() override {
		windowSizeShown_ = true;
	}
	
	void showUsingHearingAidSimulationCheckBox() override {
		usingHearingAidSimulationCheckBoxShown_ = true;
	}
	
	void showAudioFilePath() override {
		audioFilePathShown_ = true;
	}
	
	void showPlayButton() override {
		playButtonShown_ = true;
	}
	
	void showStopButton() override {
		stopButtonShown_ = true;
	}
	
	void showSaveButton() override {
		saveButtonShown_ = true;
	}
	
	void showLevel_dB_Spl() override {
		level_dB_SplShown_ = true;
	}

	void saveAudio() {
		listener_->saveAudio();
	}

	std::string audioFilePath() override {
		return audioFilePath_;
	}

	void setAudioFilePath(std::string s) override {
		audioFilePath_ = std::move(s);
	}

	void stopCalibration() {
		listener_->stopCalibration();
	}

	void playCalibration() {
		listener_->playCalibration();
	}
	
	void activateReleaseTime_ms() override {
		release_msActivated_ = true;
	};

	void activateAttackTime_ms() override {
		attack_msActivated_ = true;
	};
	
	void activateWindowSize() override {
		windowSizeActivated_ = true;
	};
	
	void activateChunkSize() override {
		chunkSizeActivated_ = true;
	};
	
	void deactivateReleaseTime_ms() override {
		release_msDeactivated_ = true;
	};
	
	void deactivateAttackTime_ms() override {
		attack_msDeactivated_ = true;
	};
	
	void deactivateWindowSize() override {
		windowSizeDeactivated_ = true;
	};
	
	void deactivateChunkSize() override {
		chunkSizeDeactivated_ = true;
	};

	void toggleHearingAidSimulation() {
		listener_->toggleUsingHearingAidSimulation();
	}

	void activateLeftDslPrescriptionFilePath() override {
		leftDslPrescriptionFilePathActivated_ = true;
	};
	
	void activateRightDslPrescriptionFilePath() override {
		rightDslPrescriptionFilePathActivated_ = true;
	};
	
	void activateBrowseForLeftDslPrescriptionButton() override {
		browseForLeftDslPrescriptionButtonActivated_ = true;
	};
	
	void activateBrowseForRightDslPrescriptionButton() override {
		browseForRightDslPrescriptionButtonActivated_ = true;
	};
	
	void deactivateLeftDslPrescriptionFilePath() override {
		leftDslPrescriptionFilePathDeactivated_ = true;
	};
	
	void deactivateRightDslPrescriptionFilePath() override {
		rightDslPrescriptionFilePathDeactivated_ = true;
	};
	
	void deactivateBrowseForLeftDslPrescriptionButton() override {
		browseForLeftDslPrescriptionButtonDeactivated_ = true;
	};
	
	void deactivateBrowseForRightDslPrescriptionButton() override {
		browseForRightDslPrescriptionButtonDeactivated_ = true;
	};

	bool usingHearingAidSimulation() override {
		return usingHearingAidSimulation_;
	}
	
	void setHearingAidSimulationOn() noexcept {
		usingHearingAidSimulation_ = true;
	}

	void setHearingAidSimulationOff() noexcept {
		usingHearingAidSimulation_ = false;
	}

	void clearActivationState() noexcept {
		browseForBrirButtonActivated_ = false;
		brirFilePathActivated_ = false;
		browseForBrirButtonDeactivated_ = false;
		brirFilePathDeactivated_ = false;
		browseForLeftDslPrescriptionButtonActivated_ = false;
		browseForRightDslPrescriptionButtonActivated_ = false;
		leftDslPrescriptionFilePathActivated_ = false;
		rightDslPrescriptionFilePathActivated_ = false;
		leftDslPrescriptionFilePathDeactivated_ = false;
		rightDslPrescriptionFilePathDeactivated_ = false;
		browseForLeftDslPrescriptionButtonDeactivated_ = false;
		browseForRightDslPrescriptionButtonDeactivated_ = false;
		release_msDeactivated_ = false;
		attack_msDeactivated_ = false;
		windowSizeDeactivated_ = false;
		chunkSizeDeactivated_ = false;
		chunkSizeActivated_ = false;
		windowSizeActivated_ = false;
		attack_msActivated_ = false;
		release_msActivated_ = false;
	}

	bool usingSpatialization() override {
		return usingSpatialization_;
	}

	void activateBrowseForBrirButton() override {
		browseForBrirButtonActivated_ = true;
	}

	void activateBrirFilePath() override {
		brirFilePathActivated_ = true;
	}

	auto browseForBrirButtonActivated() const noexcept {
		return browseForBrirButtonActivated_;
	}

	auto brirFilePathActivated() const noexcept {
		return brirFilePathActivated_;
	}

	void setSpatializationOn() noexcept {
		usingSpatialization_ = true;
	}

	void deactivateBrowseForBrirButton() override {
		browseForBrirButtonDeactivated_ = true;
	}

	void deactivateBrirFilePath() override {
		brirFilePathDeactivated_ = true;
	}

	auto browseForBrirButtonDeactivated() const noexcept {
		return browseForBrirButtonDeactivated_;
	}

	auto brirFilePathDeactivated() const noexcept {
		return brirFilePathDeactivated_;
	}

	void toggleSpatialization() {
		listener_->toggleUsingSpatialization();
	}

	void setSpatializationOff() noexcept {
		usingSpatialization_ = false;
	}

	void setSubjectId(std::string s) {
		subjectId_ = std::move(s);
	}

	void setTesterId(std::string s) {
		testerId_ = std::move(s);
	}

	std::string subjectId() override {
		return subjectId_;
	}

	std::string testerId() override {
		return testerId_;
	}

	void subscribe(EventListener * listener) override {
		listener_ = listener;
	}

	auto listener() const noexcept {
		return listener_;
	}

	void runEventLoop() override {
		runningEventLoop_ = true;
	}

	auto runningEventLoop() const noexcept {
		return runningEventLoop_;
	}

	std::string browseForOpeningFile(
		std::vector<std::string> filters
	) override {
		if (cancelOpeningForSavingFile_)
			browseCancelled_ = true;
		browseFiltersForOpeningFile_ = std::move(filters);
		return browseForOpeningFileResult_;
	}

	void setBrowseForOpeningFileResult(std::string p) {
		browseForOpeningFileResult_ = std::move(p);
	}

	bool browseCancelled() override {
		return browseCancelled_;
	}

	void setBrowseCancelled() noexcept {
		browseCancelled_ = true;
	}

	void browseForTestFile() {
		listener_->browseForTestFile();
	}

	void browseForLeftDslPrescription() {
		listener_->browseForLeftDslPrescription();
	}

	void cancelWhenBrowsingForOpeningFile() {
		cancelOpeningForSavingFile_ = true;
	}

	void cancelWhenBrowsingForSavingFile() {
		cancelBrowsingForSavingFile_ = true;
	}

	std::string browseForSavingFile(
		std::vector<std::string> filters
	) override {
		if (cancelBrowsingForSavingFile_)
			browseCancelled_ = true;
		browseFiltersForSavingFile_ = std::move(filters);
		return browseForSavingFileResult_;
	}

	auto browseFiltersForSavingFile() const {
		return browseFiltersForSavingFile_;
	}

	void setBrowseForSavingFileResult(std::string p) {
		browseForSavingFileResult_ = std::move(p);
	}

	void setTestFilePath(std::string p) override {
		testFilePath_ = std::move(p);
	}

	std::string testFilePath() override {
		return testFilePath_;
	}

	void setLeftDslPrescriptionFilePath(std::string p) override {
		leftDslPrescriptionFilePath_ = std::move(p);
	}

	std::string leftDslPrescriptionFilePath() override {
		return leftDslPrescriptionFilePath_;
	}

	void browseForRightDslPrescription() {
		listener_->browseForRightDslPrescription();
	}

	std::string rightDslPrescriptionFilePath() override {
		return rightDslPrescriptionFilePath_;
	}

	void setRightDslPrescriptionFilePath(std::string p) override {
		rightDslPrescriptionFilePath_ = std::move(p);
	}

	void browseForStimulusList() {
		listener_->browseForStimulusList();
	}

	void browseForBrir() {
		listener_->browseForBrir();
	}

	void setBrirFilePath(std::string p) override {
		brirFilePath_ = std::move(p);
	}

	std::string brirFilePath() override {
		return brirFilePath_;
	}

	auto browseForOpeningFileFilters() const {
		return browseFiltersForOpeningFile_;
	}

	void setLevel_dB_Spl(std::string level) {
		level_dB_Spl_ = std::move(level);
	}

	std::string level_dB_Spl() override {
		return level_dB_Spl_;
	}

	void setAttack_ms(std::string a) {
		attack_ms_ = std::move(a);
	}

	std::string attack_ms() override {
		return attack_ms_;
	}

	void setRelease_ms(std::string r) {
		release_ms_ = std::move(r);
	}

	std::string release_ms() override {
		return release_ms_;
	}

	void setWindowSize(std::string s) {
		windowSize_ = std::move(s);
	}

	std::string windowSize() override {
		return windowSize_;
	}

	void setChunkSize(std::string s) {
		chunkSize_ = std::move(s);
	}

	std::string chunkSize() override {
		return chunkSize_;
	}

	auto errorMessage() const {
		return errorMessage_;
	}

	void showErrorDialog(std::string message) override {
		errorMessage_ = std::move(message);
	}

	auto audioDeviceMenuItems() const {
		return audioDeviceMenuItems_;
	}

	void populateAudioDeviceMenu(std::vector<std::string> d) override {
		audioDeviceMenuItems_ = std::move(d);
	}

	void setAudioDevice(std::string s) {
		audioDevice_ = std::move(s);
	}

	std::string audioDevice() override {
		return audioDevice_;
	}

	void setBrowseDirectory(std::string d) {
		browseDirectory_ = std::move(d);
	}

	std::string stimulusList() override {
		return stimulusList_;
	}

	std::string browseForDirectory() override {
		return browseDirectory_;
	}

	void setStimulusList(std::string d) override {
		stimulusList_ = std::move(d);
	}

	void confirmTestSetup() {
		listener_->confirmTestSetup();
	}

	auto testerViewShown() const noexcept {
		return testerViewShown_;
	}

	void playNextTrial() {
		listener_->playNextTrial();
	}

	auto testerViewHidden() const noexcept {
		return testerViewHidden_;
	}

	auto chunkSizeItems() const {
		return chunkSizeItems_;
	}

	auto windowSizeItems() const {
		return windowSizeItems_;
	}

	void populateChunkSizeMenu(std::vector<std::string> v) override {
		chunkSizeItems_ = std::move(v);
	}

	void populateWindowSizeMenu(std::vector<std::string> v) override {
		windowSizeItems_ = std::move(v);
	}
};