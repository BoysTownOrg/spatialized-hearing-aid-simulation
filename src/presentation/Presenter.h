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
	void browseForLeftDslPrescription() override;
	void browseForRightDslPrescription() override;
	void browseForAudio() override;
	void browseForBrir() override;
	void confirmTestSetup() override;
	void playTrial() override;

private:
	std::vector<std::string> preferredProcessingSizes();
	void applyIfBrowseNotCancelled(
		std::string,
		std::function<void(std::string)>
	);
	void prepareTest();
	void initializeModel();
	RUNTIME_ERROR(BadInput);
	double convertToDouble(std::string x, std::string identifier);
	int convertToPositiveInteger(std::string x, std::string identifier);
	int convertToInteger(std::string x, std::string identifier);
	void playTrial_();
	void switchViewIfTestComplete();
};
