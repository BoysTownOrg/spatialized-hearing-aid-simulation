#pragma once

#include <presentation/View.h>
#pragma warning (push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl.H>
#pragma warning (pop)

struct Fl_ChoiceFacade : public Fl_Choice {
	Fl_ChoiceFacade(int, int, int, int, const char * = {});
	void populate(std::vector<std::string> items);
};

struct FltkSpatialization : public Fl_Group {
	FltkSpatialization(int, int, int, int, const char * = {});
	Fl_Input brirFilePath_;
	Fl_Button browseBrir;
};

struct FltkHearingAidSimulationGroup : public Fl_Group {
	FltkHearingAidSimulationGroup(int, int, int, int, const char * = {});
	Fl_Float_Input attack_ms_;
	Fl_Float_Input release_ms_;
	Fl_Input leftPrescriptionFilePath_;
	Fl_Input rightPrescriptionFilePath_;
	Fl_Button browseLeftPrescription;
	Fl_Button browseRightPrescription;
	Fl_ChoiceFacade windowSize_;
	Fl_ChoiceFacade chunkSize_;
};

struct FltkCalibration : public Fl_Group {
	FltkCalibration(int, int, int, int, const char * = {});
	Fl_Input audioFilePath_;
	Fl_Float_Input level_dB_Spl_;
	Fl_Button browseForAudioFile;
	Fl_Button play;
	Fl_Button stop;
	Fl_Button save;
};

struct FltkTestSetupGroup : Fl_Group {
	FltkTestSetupGroup(int, int, int, int, const char * = {});
	FltkHearingAidSimulationGroup hearingAidSimulation;
	FltkCalibration calibration;
	FltkSpatialization spatialization;
	Fl_Input subjectId_;
	Fl_Input testerId_;
	Fl_Input testFilePath_;
	Fl_Input stimulusList_;
	Fl_Button browseTestFilePath;
	Fl_Button browseForStimulusList;
	Fl_Button confirm;
	Fl_Check_Button usingSpatialization_;
	Fl_Check_Button usingHearingAidSimulation_;
};

struct FltkWindow : public Fl_Double_Window {
	FltkWindow(int, int, int, int, const char * = {});
	FltkTestSetupGroup testSetup;
	Fl_ChoiceFacade audioDevice_;
	Fl_Button playNextTrial;
};

class FltkView : public View {
public:
	class FltkTestSetup : public TestSetup {
	public:
		explicit FltkTestSetup(FltkTestSetupGroup *);
		void setStimulusList(std::string) override;
		void setLeftDslPrescriptionFilePath(std::string) override;
		void setRightDslPrescriptionFilePath(std::string) override;
		void setBrirFilePath(std::string) override;
		void setTestFilePath(std::string) override;
		void setAudioFilePath(std::string) override;
		std::string subjectId() override;
		std::string testerId() override;
		std::string testFilePath() override;
		std::string stimulusList() override;
		std::string audioFilePath() override;
		std::string leftDslPrescriptionFilePath() override;
		std::string rightDslPrescriptionFilePath() override;
		std::string brirFilePath() override;
		std::string level_dB_Spl() override;
		std::string attack_ms() override;
		std::string release_ms() override;
		std::string windowSize() override;
		std::string chunkSize() override;
		void hide() override;
		void show() override;
	private:
		FltkTestSetupGroup *view;
	};

	FltkView();
	TestSetup *testSetup() override;
	void subscribe(EventListener * listener) override;
	void runEventLoop() override;
	std::string browseForOpeningFile(std::vector<std::string> filters) override;
	std::string browseForSavingFile(std::vector<std::string> filters) override;
	std::string browseForDirectory() override;
	bool browseCancelled() override;
	std::string audioDevice() override;
	bool usingSpatialization() override;
	void showErrorDialog(std::string message) override;
	void populateAudioDeviceMenu(std::vector<std::string> items) override;
	void populateChunkSizeMenu(std::vector<std::string> items) override;
	void populateWindowSizeMenu(std::vector<std::string> items) override;
	void deactivateBrowseForBrirButton() override;
	void deactivateBrirFilePath() override;
	void activateBrowseForBrirButton() override;
	void activateBrirFilePath() override;
	bool usingHearingAidSimulation() override;
	void activateLeftDslPrescriptionFilePath() override;
	void activateRightDslPrescriptionFilePath() override;
	void activateBrowseForLeftDslPrescriptionButton() override;
	void activateBrowseForRightDslPrescriptionButton() override;
	void deactivateLeftDslPrescriptionFilePath() override;
	void deactivateRightDslPrescriptionFilePath() override;
	void deactivateBrowseForLeftDslPrescriptionButton() override;
	void deactivateBrowseForRightDslPrescriptionButton() override;
	void activateReleaseTime_ms() override;
	void activateAttackTime_ms() override;
	void activateWindowSize() override;
	void activateChunkSize() override;
	void deactivateReleaseTime_ms() override;
	void deactivateAttackTime_ms() override;
	void deactivateWindowSize() override;
	void deactivateChunkSize() override;
	void hidePlayNextTrialButton() override;
	void showPlayNextTrialButton() override;

private:
	void registerCallbacks();
	void turnOnHearingAidSimulation();
	void turnOnSpatialization();
	static void onBrowseTestFile(Fl_Widget *, void *);
	static void onBrowseLeftPrescription(Fl_Widget *, void *);
	static void onBrowseRightPrescription(Fl_Widget *, void *);
	static void onBrowseStimulusList(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
	static void onConfirmTestSetup(Fl_Widget *, void *);
	static void onPlayTrial(Fl_Widget *, void *);
	static void onToggleSpatialization(Fl_Widget *, void *);
	static void onToggleHearingAidSimulation(Fl_Widget *, void *);
	static void onPlayCalibration(Fl_Widget *, void *);
	static void onStopCalibration(Fl_Widget *, void *);
	static void onSaveAudio(Fl_Widget *, void *);

	FltkWindow window;
	FltkTestSetup testSetup_;
	EventListener *listener{};
	int browseResult{};
};
