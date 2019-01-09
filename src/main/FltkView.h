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

struct FltkSetupView : public Fl_Group {
	Fl_Input _leftPrescriptionFilePath;
	Fl_Input _rightPrescriptionFilePath;
	Fl_Input _audioDirectory;
	Fl_Input _brirFilePath;
	Fl_Float_Input _attack_ms;
	Fl_Float_Input _release_ms;
	Fl_Input _windowSize;
	Fl_Input _chunkSize;
	Fl_Button browseLeftPrescription;
	Fl_Button browseRightPrescription;
	Fl_Button browseAudio;
	Fl_Button browseBrir;
	Fl_Button confirm;
	FltkSetupView();
};

struct FltkTesterView : public Fl_Group {
	Fl_Float_Input _level_dB_Spl;
	Fl_Choice _audioDevice;
	Fl_Button play;
	FltkTesterView();
};

struct FltkWindow : public Fl_Double_Window {
	FltkSetupView setupView;
	FltkTesterView testerView;
	FltkWindow();
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
	void showTestSetup() override;
	void hideTestSetup() override;
	void showTesterView() override;

private:
	static void onBrowseLeftPrescription(Fl_Widget *, void *);
	static void onBrowseRightPrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
	static void onConfirmTestSetup(Fl_Widget *, void *);
	static void onPlay(Fl_Widget *, void *);
};
