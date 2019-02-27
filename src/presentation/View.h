#pragma once

#include <common-includes/Interface.h>
#include <string>
#include <vector>

class View {
public:
	class TestSetup {
	public:
		INTERFACE_OPERATIONS(TestSetup);
		virtual void show() = 0;
		virtual void hide() = 0;
		virtual std::string subjectId() = 0;
		virtual std::string testerId() = 0;
		virtual std::string testFilePath() = 0;
	};

	class EventListener {
	public:
		INTERFACE_OPERATIONS(EventListener);
		virtual void browseForTestFile() = 0;
		virtual void browseForAudioFile() = 0;
		virtual void browseForLeftDslPrescription() = 0;
		virtual void browseForRightDslPrescription() = 0;
		virtual void browseForStimulusList() = 0;
		virtual void browseForBrir() = 0;
		virtual void toggleUsingSpatialization() = 0;
		virtual void toggleUsingHearingAidSimulation() = 0;
		virtual void playCalibration() = 0;
		virtual void stopCalibration() = 0;
		virtual void confirmTestSetup() = 0;
		virtual void playNextTrial() = 0;
		virtual void saveAudio() = 0;
	};

	INTERFACE_OPERATIONS(View);
	virtual TestSetup *testSetup() = 0;
	virtual void subscribe(EventListener *listener) = 0;
	virtual void runEventLoop() = 0;
	virtual std::string browseForOpeningFile(std::vector<std::string> filters) = 0;
	virtual std::string browseForSavingFile(std::vector<std::string> filters) = 0;
	virtual std::string browseForDirectory() = 0;
	virtual bool browseCancelled() = 0;
	virtual void setTestFilePath(std::string) = 0;
	virtual void setLeftDslPrescriptionFilePath(std::string) = 0;
	virtual void setRightDslPrescriptionFilePath(std::string) = 0;
	virtual void setStimulusList(std::string) = 0;
	virtual void setBrirFilePath(std::string) = 0;
	virtual void setAudioFilePath(std::string) = 0;
	virtual std::string leftDslPrescriptionFilePath() = 0;
	virtual std::string rightDslPrescriptionFilePath() = 0;
	virtual std::string stimulusList() = 0;
	virtual std::string audioFilePath() = 0;
	virtual std::string brirFilePath() = 0;
	virtual std::string audioDevice() = 0;
	virtual std::string level_dB_Spl() = 0;
	virtual std::string attack_ms() = 0;
	virtual std::string release_ms() = 0;
	virtual std::string windowSize() = 0;
	virtual std::string chunkSize() = 0;
	virtual bool usingSpatialization() = 0;
	virtual bool usingHearingAidSimulation() = 0;
	virtual void showErrorDialog(std::string message) = 0;
	virtual void populateAudioDeviceMenu(std::vector<std::string> items) = 0;
	virtual void populateChunkSizeMenu(std::vector<std::string> items) = 0;
	virtual void populateWindowSizeMenu(std::vector<std::string> items) = 0;
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
	virtual void hidePlayNextTrialButton() = 0;
	virtual void showPlayNextTrialButton() = 0;
};
