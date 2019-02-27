#pragma once

#include <presentation/View.h>

class ViewStub : public View {
public:
	class TestSetupStub : public TestSetup {
	public:
		std::string testFilePath_{};
		std::string subjectId_{};
		std::string testerId_{};
		std::string leftDslPrescriptionFilePath_{};
		std::string rightDslPrescriptionFilePath_{};
		std::string brirFilePath_{};
		std::string audioFilePath_{};
		std::string level_dB_Spl_{ "0" };
		std::string attack_ms_{ "0" };
		std::string release_ms_{ "0" };
		std::string windowSize_{ "0" };
		std::string chunkSize_{ "0" };
		std::string stimulusList_{};
		bool shown_{};
		bool hidden_{};

		void hide() override {
			hidden_ = true;
		}

		void show() override {
			shown_ = true;
		}

		std::string subjectId() override {
			return subjectId_;
		}

		std::string testerId() override {
			return testerId_;
		}

		std::string testFilePath() override {
			return testFilePath_;
		}

		std::string leftDslPrescriptionFilePath() override {
			return leftDslPrescriptionFilePath_;
		}

		std::string rightDslPrescriptionFilePath() override {
			return rightDslPrescriptionFilePath_;
		}

		std::string brirFilePath() override {
			return brirFilePath_;
		}

		std::string level_dB_Spl() override {
			return level_dB_Spl_;
		}

		std::string attack_ms() override {
			return attack_ms_;
		}

		std::string release_ms() override {
			return release_ms_;
		}

		std::string windowSize() override {
			return windowSize_;
		}

		std::string chunkSize() override {
			return chunkSize_;
		}

		std::string stimulusList() override {
			return stimulusList_;
		}

		std::string audioFilePath() override {
			return audioFilePath_;
		}

		void setAudioFilePath(std::string s) override {
			audioFilePath_ = std::move(s);
		}

		void setTestFilePath(std::string p) override {
			testFilePath_ = std::move(p);
		}

		void setLeftDslPrescriptionFilePath(std::string p) override {
			leftDslPrescriptionFilePath_ = std::move(p);
		}

		void setRightDslPrescriptionFilePath(std::string p) override {
			rightDslPrescriptionFilePath_ = std::move(p);
		}

		void setBrirFilePath(std::string p) override {
			brirFilePath_ = std::move(p);
		}

		void setLevel_dB_Spl(std::string level) {
			level_dB_Spl_ = std::move(level);
		}

		void setAttack_ms(std::string a) {
			attack_ms_ = std::move(a);
		}

		void setRelease_ms(std::string r) {
			release_ms_ = std::move(r);
		}

		void setWindowSize(std::string s) {
			windowSize_ = std::move(s);
		}

		void setChunkSize(std::string s) {
			chunkSize_ = std::move(s);
		}

		void setStimulusList(std::string d) override {
			stimulusList_ = std::move(d);
		}
	};

	TestSetupStub testSetup_{};
	std::vector<std::string> browseFiltersForOpeningFile_{};
	std::vector<std::string> browseFiltersForSavingFile_{};
	std::vector<std::string> audioDeviceMenuItems_{};
	std::vector<std::string> chunkSizeItems_{};
	std::vector<std::string> windowSizeItems_{};
	std::string audioDevice_{};
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
	bool playNextTrialButtonShown_{};
	bool playNextTrialButtonHidden_{};
	bool cancelBrowsingForSavingFile_{};
	bool cancelOpeningForSavingFile_{};

	void browseForBrir() {
		listener_->browseForBrir();
	}

	void browseForRightDslPrescription() {
		listener_->browseForRightDslPrescription();
	}

	TestSetup *testSetup() override {
		return &testSetup_;
	}

	void browseForAudioFile() {
		listener_->browseForAudioFile();
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

	void saveAudio() {
		listener_->saveAudio();
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

	void setSpatializationOn() noexcept {
		usingSpatialization_ = true;
	}

	void deactivateBrowseForBrirButton() override {
		browseForBrirButtonDeactivated_ = true;
	}

	void deactivateBrirFilePath() override {
		brirFilePathDeactivated_ = true;
	}

	void toggleSpatialization() {
		listener_->toggleUsingSpatialization();
	}

	void setSpatializationOff() noexcept {
		usingSpatialization_ = false;
	}

	void setSubjectId(std::string s) {
		testSetup_.subjectId_ = std::move(s);
	}

	void setTesterId(std::string s) {
		testSetup_.testerId_ = std::move(s);
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

	void browseForStimulusList() {
		listener_->browseForStimulusList();
	}

	auto browseForOpeningFileFilters() const {
		return browseFiltersForOpeningFile_;
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

	std::string browseForDirectory() override {
		return browseDirectory_;
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