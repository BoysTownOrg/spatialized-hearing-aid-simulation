#pragma once

#include "View.h"
#include "Model.h"
#include <common-includes/RuntimeError.h>
#include <memory>
#include <functional>

#ifdef PRESENTATION_EXPORTS
	#define PRESENTATION_API __declspec(dllexport)
#else
	#define PRESENTATION_API __declspec(dllimport)
#endif

class Presenter : public EventListener {
	Model *model;
	View *view;
public:
	PRESENTATION_API Presenter(Model *model, View *view);
	PRESENTATION_API void run();
	void browseForTestFile() override;
	void browseForLeftDslPrescription() override;
	void browseForRightDslPrescription() override;
	void browseForAudio() override;
	void browseForBrir() override;
	void confirmTestSetup() override;
	void playTrial() override;
	void toggleUsingSpatialization() override;
	void toggleUsingHearingAidSimulation() override;
	void calibrate() override;
	void playCalibration() override;
	void stopCalibration() override;

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
	void hideTestSetupView();
	Model::TestParameters testParameters();
	Model::TrialParameters trialParameters();
	Model::ProcessingParameters processingParameters();
	RUNTIME_ERROR(BadInput);
	double convertToDouble(std::string x, std::string identifier);
	int convertToPositiveInteger(std::string x, std::string identifier);
	int convertToInteger(std::string x, std::string identifier);
	void playTrial_();
	void playCalibration_();
	void switchViewIfTestComplete();
};
