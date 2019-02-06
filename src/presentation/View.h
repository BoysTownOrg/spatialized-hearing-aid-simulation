#pragma once

#include <common-includes/Interface.h>
#include <string>
#include <vector>

class EventListener {
public:
	INTERFACE_OPERATIONS(EventListener);
	virtual void browseForTestFile() = 0;
	virtual void browseForLeftDslPrescription() = 0;
	virtual void browseForRightDslPrescription() = 0;
	virtual void browseForAudio() = 0;
	virtual void browseForBrir() = 0;
	virtual void toggleUsingSpatialization() = 0;
	virtual void toggleUsingHearingAidSimulation() = 0;
	virtual void playCalibration() = 0;
	virtual void stopCalibration() = 0;
	virtual void confirmTestSetup() = 0;
	virtual void playTrial() = 0;
};

class View {
public:
	INTERFACE_OPERATIONS(View);
	virtual void subscribe(EventListener *listener) = 0;
	virtual void runEventLoop() = 0;
	virtual std::string browseForOpeningFile(std::vector<std::string> filters) = 0;
	virtual std::string browseForSavingFile(std::vector<std::string> filters) = 0;
	virtual std::string browseForDirectory() = 0;
	virtual bool browseCancelled() = 0;
	virtual void setTestFilePath(std::string) = 0;
	virtual void setLeftDslPrescriptionFilePath(std::string) = 0;
	virtual void setRightDslPrescriptionFilePath(std::string) = 0;
	virtual void setAudioDirectory(std::string) = 0;
	virtual void setBrirFilePath(std::string) = 0;
	virtual std::string subjectId() = 0;
	virtual std::string testerId() = 0;
	virtual std::string testFilePath() = 0;
	virtual std::string leftDslPrescriptionFilePath() = 0;
	virtual std::string rightDslPrescriptionFilePath() = 0;
	virtual std::string audioDirectory() = 0;
	virtual std::string audioFilePath() = 0;
	virtual std::string brirFilePath() = 0;
	virtual std::string audioDevice() = 0;
	virtual std::string level_dB_Spl() = 0;
	virtual std::string attack_ms() = 0;
	virtual std::string release_ms() = 0;
	virtual std::string windowSize() = 0;
	virtual std::string chunkSize() = 0;
	virtual std::string calibrationLevel_dB_Spl() = 0;
	virtual bool usingSpatialization() = 0;
	virtual bool usingHearingAidSimulation() = 0;
	virtual void showErrorDialog(std::string message) = 0;
	virtual void populateAudioDeviceMenu(std::vector<std::string> items) = 0;
	virtual void populateChunkSizeMenu(std::vector<std::string> items) = 0;
	virtual void populateWindowSizeMenu(std::vector<std::string> items) = 0;
	virtual void showTesterView() = 0;
	virtual void hideTesterView() = 0;
	virtual void deactivateBrowseForBrirButton() = 0;
	virtual void deactivateBrirFilePath() = 0;
	virtual void activateBrowseForBrirButton() = 0;
	virtual void activateBrirFilePath() = 0;
	virtual void activateLeftDslPrescriptionFilePath() = 0;
	virtual void activateRightDslPrescriptionFilePath() = 0;
	virtual void activateBrowseForLeftDslPrescriptionButton() = 0;
	virtual void activateBrowseForRightDslPrescriptionButton() = 0;
	virtual void deactivateLeftDslPrescriptionFilePath() = 0;
	virtual void deactivateRightDslPrescriptionFilePath() = 0;
	virtual void deactivateBrowseForLeftDslPrescriptionButton() = 0;
	virtual void deactivateBrowseForRightDslPrescriptionButton() = 0;
	virtual void activateReleaseTime_ms() = 0;
	virtual void activateAttackTime_ms() = 0;
	virtual void activateWindowSize() = 0;
	virtual void activateChunkSize() = 0;
	virtual void deactivateReleaseTime_ms() = 0;
	virtual void deactivateAttackTime_ms() = 0;
	virtual void deactivateWindowSize() = 0;
	virtual void deactivateChunkSize() = 0;
	virtual void hideSubjectId() = 0;
	virtual void hideTesterId() = 0;
	virtual void hideStimulusList() = 0;
	virtual void hideOutputFilePath() = 0;
	virtual void hideConfirmButton() = 0;
	virtual void hideBrirFilePath() = 0;
	virtual void hideBrowseForBrirButton() = 0;
	virtual void hideUsingSpatializationCheckBox() = 0;
	virtual void hideLeftDslPrescriptionFilePath() = 0;
	virtual void hideBrowseForLeftDslPrescriptionButton() = 0;
	virtual void hideRightDslPrescriptionFilePath() = 0;
	virtual void hideBrowseForRightDslPrescriptionButton() = 0;
	virtual void hideAttack_ms() = 0;
	virtual void hideRelease_ms() = 0;
	virtual void hideChunkSize() = 0;
	virtual void hideWindowSize() = 0;
	virtual void hideUsingHearingAidSimulationCheckBox() = 0;
	virtual void hideAudioFilePath() = 0;
	virtual void hidePlayButton() = 0;
	virtual void hideStopButton() = 0;
	virtual void hideLevel_dB_Spl() = 0;
	
	virtual void showSubjectId() = 0;
	virtual void showTesterId() = 0;
	virtual void showStimulusList() = 0;
	virtual void showOutputFilePath() = 0;
	virtual void showConfirmButton() = 0;
	virtual void showBrirFilePath() = 0;
	virtual void showBrowseForBrirButton() = 0;
	virtual void showUsingSpatializationCheckBox() = 0;
	virtual void showLeftDslPrescriptionFilePath() = 0;
	virtual void showBrowseForLeftDslPrescriptionButton() = 0;
	virtual void showRightDslPrescriptionFilePath() = 0;
	virtual void showBrowseForRightDslPrescriptionButton() = 0;
	virtual void showAttack_ms() = 0;
	virtual void showRelease_ms() = 0;
	virtual void showChunkSize() = 0;
	virtual void showWindowSize() = 0;
	virtual void showUsingHearingAidSimulationCheckBox() = 0;
	virtual void showAudioFilePath() = 0;
	virtual void showPlayButton() = 0;
	virtual void showStopButton() = 0;
	virtual void showLevel_dB_Spl() = 0;
};
