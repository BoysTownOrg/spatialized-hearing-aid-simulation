#pragma once

#include <presentation/Presenter.h>
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
	Fl_Input _audioFilePath;
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
	Presenter *presenter{};
	int browseResult{};
public:
	FltkWindow();
	void setPresenter(Presenter *) override;
	void runEventLoop() override;
	std::string browseForFile(std::vector<std::string> filters) override;
	bool browseCancelled() override;
	void setLeftDslPrescriptionFilePath(std::string) override;
	void setRightDslPrescriptionFilePath(std::string) override;
	void setAudioFilePath(std::string) override;
	void setBrirFilePath(std::string) override;
	std::string leftDslPrescriptionFilePath() const override;
	std::string rightDslPrescriptionFilePath() const override;
	std::string audioFilePath() const override;
	std::string brirFilePath() const override;
	std::string audioDevice() const override;
	std::string level_dB_Spl() const override;
	std::string attack_ms() const override;
	std::string release_ms() const override;
	std::string windowSize() const override;
	std::string chunkSize() const override;
	void showErrorDialog(std::string message) override;
	void populateAudioDeviceMenu(std::vector<std::string> items) override;

private:
	static void onBrowseLeftPrescription(Fl_Widget *, void *);
	static void onBrowseRightPrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
	static void onPlay(Fl_Widget *, void *);
};
