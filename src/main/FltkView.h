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

struct FltkSetupView : public Fl_Group {
	FltkSetupView(int, int, int, int, const char * = {});
	Fl_Input subjectId_;
	Fl_Input testerId_;
	Fl_Input testFilePath_;
	Fl_Input leftPrescriptionFilePath_;
	Fl_Input rightPrescriptionFilePath_;
	Fl_Input audioDirectory_;
	Fl_Input brirFilePath_;
	Fl_Float_Input attack_ms_;
	Fl_Float_Input release_ms_;
	Fl_ChoiceFacade windowSize_;
	Fl_ChoiceFacade chunkSize_;
	Fl_Button browseTestFilePath;
	Fl_Button browseLeftPrescription;
	Fl_Button browseRightPrescription;
	Fl_Button browseAudio;
	Fl_Button browseBrir;
	Fl_Button confirm;
	Fl_Check_Button usingSpatialization_;
	Fl_Check_Button usingHearingAidSimulation_;
};

struct FltkTesterView : public Fl_Group {
	FltkTesterView(int, int, int, int, const char * = {});
	Fl_Float_Input level_dB_Spl_;
	Fl_ChoiceFacade audioDevice_;
	Fl_Button play;
};

struct FltkWindow : public Fl_Double_Window {
	FltkWindow(int, int, int, int, const char * = {});
	FltkSetupView setupView;
	FltkTesterView testerView;
};

class FltkView : public View {
	FltkWindow window;
	EventListener *listener{};
	int browseResult{};
public:
	FltkView();
	void subscribe(EventListener * listener) override;
	void runEventLoop() override;
	std::string browseForOpeningFile(std::vector<std::string> filters) override;
	std::string browseForSavingFile(std::vector<std::string> filters) override;
	std::string browseForDirectory() override;
	bool browseCancelled() override;
	void setAudioDirectory(std::string) override;
	void setLeftDslPrescriptionFilePath(std::string) override;
	void setRightDslPrescriptionFilePath(std::string) override;
	void setBrirFilePath(std::string) override;
	void setTestFilePath(std::string) override;
	std::string subjectId() override;
	std::string testerId() override;
	std::string testFilePath() override;
	std::string audioDirectory() override;
	std::string leftDslPrescriptionFilePath() override;
	std::string rightDslPrescriptionFilePath() override;
	std::string brirFilePath() override;
	std::string audioDevice() override;
	std::string level_dB_Spl() override;
	std::string attack_ms() override;
	std::string release_ms() override;
	std::string windowSize() override;
	std::string chunkSize() override;
	bool usingSpatialization() override;
	void showErrorDialog(std::string message) override;
	void populateAudioDeviceMenu(std::vector<std::string> items) override;
	void populateChunkSizeMenu(std::vector<std::string> items) override;
	void populateWindowSizeMenu(std::vector<std::string> items) override;
	void showTestSetup() override;
	void hideTestSetup() override;
	void showTesterView() override;
	void hideTesterView() override;
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

private:
	static void onBrowseTestFile(Fl_Widget *, void *);
	static void onBrowseLeftPrescription(Fl_Widget *, void *);
	static void onBrowseRightPrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
	static void onConfirmTestSetup(Fl_Widget *, void *);
	static void onPlay(Fl_Widget *, void *);
	static void onToggleSpatialization(Fl_Widget *, void *);
	static void onToggleHearingAidSimulation(Fl_Widget *, void *);
};
