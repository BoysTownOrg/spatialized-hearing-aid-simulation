#pragma once

#include <presentation/Presenter.h>
#define WIN32
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl.H>

class FltkWindow : public View
{
	// FLTK forms widget groups at construction.
	// Therefore window is declared first.
	Fl_Double_Window window;
	Fl_Input _prescriptionFilePath;
	Fl_Input _audioFilePath;
	Fl_Input _brirFilePath;
	Fl_Button browsePrescription;
	Fl_Button browseAudio;
	Fl_Button browseBrir;
	Presenter *presenter{};
	int browseResult{};
public:
	FltkWindow();
	void setPresenter(Presenter *) override;
	void runEventLoop() override;
	std::string browseForFile(std::vector<std::string> filters) override;
	bool browseCancelled() override;
	void setLeftDslPrescriptionFilePath(std::string) override;
	void setAudioFilePath(std::string) override;
	void setBrirFilePath(std::string) override;
	std::string leftDslPrescriptionFilePath() const override;
	std::string audioFilePath() const override;
	std::string brirFilePath() const override;

private:
	static void onBrowsePrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
};
