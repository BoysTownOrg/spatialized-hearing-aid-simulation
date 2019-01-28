#pragma once

#include <presentation/View.h>

class ViewStub : public View {
	std::vector<std::string> browseFilters_{};
	std::vector<std::string> audioDeviceMenuItems_{};
	std::vector<std::string> chunkSizeItems_{};
	std::vector<std::string> windowSizeItems_{};
	std::string subjectId_{};
	std::string testerId_{};
	std::string audioDevice_{};
	std::string level_dB_Spl_{ "0" };
	std::string attack_ms_{ "0" };
	std::string release_ms_{ "0" };
	std::string windowSize_{ "0" };
	std::string chunkSize_{ "0" };
	std::string calibrationLevel_dB_Spl_{ "0" };
	std::string errorMessage_{};
	std::string browseFilePath_{};
	std::string browseDirectory_{};
	EventListener *listener_{};
	bool runningEventLoop_{};
	bool browseCancelled_{};
	bool testSetupShown_{};
	bool testSetupHidden_{};
	bool testerViewShown_{};
	bool testerViewHidden_{};
	bool usingSpatialization_{};
	bool brirFilePathDeactivated_{};
	bool browseForBrirButtonDeactivated_{};
	bool brirFilePathActivated_{};
	bool browseForBrirButtonActivated_{};
	bool usingHearingAidSimulation_{};
	bool calibrationShown_{};
	bool calibrationHidden_{};
public:
	std::string testFilePath_{};
	std::string audioDirectory_{};
	std::string leftDslPrescriptionFilePath_{};
	std::string rightDslPrescriptionFilePath_{};
	std::string brirFilePath_{};
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

	void hideCalibration() override {
		calibrationHidden_ = true;
	}

	bool calibrationHidden() const {
		return calibrationHidden_;
	}

	std::string calibrationLevel_dB_Spl() override {
		return calibrationLevel_dB_Spl_;
	}

	void confirmCalibration() {
		listener_->confirmCalibration();
	}

	void setCalibrationLevel_dB_Spl(std::string s) {
		calibrationLevel_dB_Spl_ = std::move(s);
	}

	void stopCalibration() {
		listener_->stopCalibration();
	}

	void playCalibration() {
		listener_->playCalibration();
	}

	void showCalibration() override {
		calibrationShown_ = true;
	}

	bool calibrationShown() const {
		return calibrationShown_;
	}

	void calibrate() {
		listener_->calibrate();
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
	
	void setHearingAidSimulationOn() {
		usingHearingAidSimulation_ = true;
	}

	void setHearingAidSimulationOff() {
		usingHearingAidSimulation_ = false;
	}

	void clearActivationState() {
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

	bool browseForBrirButtonActivated() const {
		return browseForBrirButtonActivated_;
	}

	bool brirFilePathActivated() const {
		return brirFilePathActivated_;
	}

	void setSpatializationOn() {
		usingSpatialization_ = true;
	}

	void deactivateBrowseForBrirButton() override {
		browseForBrirButtonDeactivated_ = true;
	}

	void deactivateBrirFilePath() override {
		brirFilePathDeactivated_ = true;
	}

	bool browseForBrirButtonDeactivated() const {
		return browseForBrirButtonDeactivated_;
	}

	bool brirFilePathDeactivated() const {
		return brirFilePathDeactivated_;
	}

	void toggleSpatialization() {
		listener_->toggleUsingSpatialization();
	}

	void setSpatializationOff() {
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

	EventListener *listener() const {
		return listener_;
	}

	void runEventLoop() override {
		runningEventLoop_ = true;
	}

	bool runningEventLoop() const {
		return runningEventLoop_;
	}

	std::string browseForOpeningFile(
		std::vector<std::string> filters
	) override {
		browseFilters_ = std::move(filters);
		return browseFilePath_;
	}

	void setBrowseFilePath(std::string p) {
		browseFilePath_ = std::move(p);
	}

	bool browseCancelled() override {
		return browseCancelled_;
	}

	void setBrowseCancelled() {
		browseCancelled_ = true;
	}

	void browseForTestFile() {
		listener_->browseForTestFile();
	}

	void browseForLeftDslPrescription() {
		listener_->browseForLeftDslPrescription();
	}

	std::string browseForSavingFile(
		std::vector<std::string> filters
	) override {
		browseFilters_ = std::move(filters);
		return browseFilePath_;
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

	void browseForAudio() {
		listener_->browseForAudio();
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

	std::vector<std::string> browseFilters() const {
		return browseFilters_;
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

	std::string errorMessage() const {
		return errorMessage_;
	}

	void showErrorDialog(std::string message) override {
		errorMessage_ = std::move(message);
	}

	std::vector<std::string> audioDeviceMenuItems() const {
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

	std::string audioDirectory() override {
		return audioDirectory_;
	}

	std::string browseForDirectory() override {
		return browseDirectory_;
	}

	void setAudioDirectory(std::string d) override {
		audioDirectory_ = std::move(d);
	}

	bool testSetupShown() const {
		return testSetupShown_;
	}

	void showTestSetup() override {
		testSetupShown_ = true;
	}

	void confirmTestSetup() {
		listener_->confirmTestSetup();
	}

	bool testSetupHidden() const {
		return testSetupHidden_;
	}

	void hideTestSetup() override {
		testSetupHidden_ = true;
	}

	bool testerViewShown() const {
		return testerViewShown_;
	}

	void showTesterView() override {
		testerViewShown_ = true;
	}

	void playTrial() {
		listener_->playTrial();
	}

	bool testerViewHidden() const {
		return testerViewHidden_;
	}

	std::vector<std::string> chunkSizeItems() const {
		return chunkSizeItems_;
	}

	std::vector<std::string> windowSizeItems() const {
		return windowSizeItems_;
	}

	void populateChunkSizeMenu(std::vector<std::string> v) override {
		chunkSizeItems_ = std::move(v);
	}

	void populateWindowSizeMenu(std::vector<std::string> v) override {
		windowSizeItems_ = std::move(v);
	}

	void hideTesterView() override {
		testerViewHidden_ = true;
	}
};