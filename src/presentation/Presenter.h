#pragma once

#include "View.h"
#include "Model.h"
#include <common-includes/RuntimeError.h>
#include <functional>

#ifdef _WIN32
    #ifdef PRESENTATION_EXPORTS
        #define PRESENTATION_API __declspec(dllexport)
    #else
        #define PRESENTATION_API __declspec(dllimport)
    #endif
#else
    #define PRESENTATION_API
#endif

class Presenter : public View::EventListener {
	Model *model;
	View *view;
public:
	PRESENTATION_API Presenter(Model *model, View *view);
	PRESENTATION_API void run();
	void browseForTestFile() override;
	void browseForLeftDslPrescription() override;
	void browseForRightDslPrescription() override;
	void browseForStimulusList() override;
	void browseForBrir() override;
	void confirmTestSetup() override;
	void playNextTrial() override;
	void toggleUsingSpatialization() override;
	void toggleUsingHearingAidSimulation() override;
	void playCalibration() override;
	void stopCalibration() override;
	void browseForAudioFile() override;
	void saveAudio() override;

private:
	void toggleSpatializationActivation();
	void toggleHearingAidSimulationActivation();
	void deactivateSpatialization();
	void activateSpatialization();
	void activateHearingAidSimulation();
	void deactivateHearingAidSimulation();
	void applyIfBrowseNotCancelled(
		std::string,
		std::function<void(std::string)>
	);
	void prepareNewTest();
	void showTesterView();
	void hideTestSetupView();
	void showTestSetupView();
	Model::Testing testing();
	Model::Trial trial();
	Model::SignalProcessing signalProcessing();
    RUNTIME_ERROR(BadInput)
	double convertToDouble(std::string x, std::string identifier);
	int convertToPositiveInteger(std::string x, std::string identifier);
	int convertToInteger(std::string x, std::string identifier);
	void playTrial_();
	void playCalibration_();
	void saveAudio_();
	void switchViewIfTestComplete();
	void hideTesterView();
};
