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
	void browseForLeftDslPrescription() override;
	void browseForRightDslPrescription() override;
	void browseForAudio() override;
	void browseForBrir() override;
	void newTest() override;
	void confirmTestSetup() override;
	void playTrial() override;

private:
	RUNTIME_ERROR(BadInput);
	void initializeTest();
	void browseAndUpdateIfNotCancelled(
		std::vector<std::string> filters,
		std::function<void(std::string)>);
	double convertToDouble(std::string x, std::string identifier);
	int convertToPositiveInteger(std::string x, std::string identifier);
};
