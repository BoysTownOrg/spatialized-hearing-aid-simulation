#pragma once

#include <presentation/View.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl.H>

class FltkWindow : public View
{
	// FLTK forms widget groups at construction.
	// Therefore window is declared first.
	Fl_Double_Window window;
	Fl_Input _leftPrescriptionFilePath;
	Fl_Input _rightPrescriptionFilePath;
	Fl_Input _audioDirectory;
	Fl_Input _brirFilePath;
	Fl_Float_Input _level_dB_Spl;
	Fl_Float_Input _attack_ms;
	Fl_Float_Input _release_ms;
	Fl_Input _windowSize;
	Fl_Input _chunkSize;
	Fl_Button browseLeftPrescription;
	Fl_Button browseRightPrescription;
	Fl_Button browseAudio;
	Fl_Button browseBrir;
	Fl_Choice _audioDevice;
	Fl_Button play;
	EventListener *listener{};
	int browseResult{};
public:
	FltkWindow();
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

private:
	static void onBrowseLeftPrescription(Fl_Widget *, void *);
	static void onBrowseRightPrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
	static void onPlay(Fl_Widget *, void *);
};
