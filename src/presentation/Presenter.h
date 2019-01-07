#pragma once

#include "presentation-exports.h"
#include "View.h"
#include "Model.h"
#include <common-includes/RuntimeError.h>
#include <memory>
#include <functional>

class Presenter : public EventListener {
	std::shared_ptr<Model> model;
	std::shared_ptr<View> view;
public:
	PRESENTATION_API Presenter(
		std::shared_ptr<Model> model, 
		std::shared_ptr<View> view);
	PRESENTATION_API void run();
	PRESENTATION_API void browseForLeftDslPrescription() override;
	PRESENTATION_API void browseForRightDslPrescription() override;
	PRESENTATION_API void browseForAudio() override;
	PRESENTATION_API void browseForBrir() override;
	PRESENTATION_API void newTest() override;
	PRESENTATION_API void confirmTestSetup() override;
	void playTrial() override;

private:
	RUNTIME_ERROR(BadInput);
	void browseAndUpdateIfNotCancelled(
		std::vector<std::string> filters,
		std::function<void(std::string)>);
	double convertToDouble(std::string x, std::string identifier);
	int convertToPositiveInteger(std::string x, std::string identifier);
};
