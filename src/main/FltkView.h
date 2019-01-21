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
#include <FL/Fl.H>
#pragma warning (pop)

struct Fl_ChoiceFacade : public Fl_Choice {
	Fl_ChoiceFacade(int, int, int, int, const char * = {});
	void populate(std::vector<std::string> items);
};

struct FltkSetupView : public Fl_Group {
	FltkSetupView(int, int, int, int, const char * = {});
	Fl_Input _leftPrescriptionFilePath;
	Fl_Input _rightPrescriptionFilePath;
	Fl_Input audioDirectory_;
	Fl_Input brirFilePath_;
	Fl_Float_Input attack_ms_;
	Fl_Float_Input release_ms_;
	Fl_ChoiceFacade windowSize_;
	Fl_ChoiceFacade chunkSize_;
	Fl_Button browseLeftPrescription;
	Fl_Button browseRightPrescription;
	Fl_Button browseAudio;
	Fl_Button browseBrir;
	Fl_Button confirm;
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
	std::string browseForFile(std::vector<std::string> filters) override;
	std::string browseForDirectory() override;
	bool browseCancelled() override;
	void setAudioDirectory(std::string) override;
	void setLeftDslPrescriptionFilePath(std::string) override;
	void setRightDslPrescriptionFilePath(std::string) override;
	void setBrirFilePath(std::string) override;
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
	void showErrorDialog(std::string message) override;
	void populateAudioDeviceMenu(std::vector<std::string> items) override;
	void populateChunkSizeMenu(std::vector<std::string> items) override;
	void populateWindowSizeMenu(std::vector<std::string> items) override;
	void showTestSetup() override;
	void hideTestSetup() override;
	void showTesterView() override;
	void hideTesterView() override;

private:
	static void onBrowseLeftPrescription(Fl_Widget *, void *);
	static void onBrowseRightPrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
	static void onConfirmTestSetup(Fl_Widget *, void *);
	static void onPlay(Fl_Widget *, void *);
};
