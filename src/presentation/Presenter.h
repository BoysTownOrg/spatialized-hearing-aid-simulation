#pragma once

#include "presentation-exports.h"
#include "Model.h"
#include "View.h"
#include <memory>
#include <functional>

class Presenter {
	std::shared_ptr<Model> model;
	std::shared_ptr<View> view;
public:
	PRESENTATION_API Presenter(
		std::shared_ptr<Model> model, 
		std::shared_ptr<View> view);
	PRESENTATION_API void loop();
	PRESENTATION_API void browseForLeftDslPrescription();
	PRESENTATION_API void browseForRightDslPrescription();
	PRESENTATION_API void browseForAudio();
	PRESENTATION_API void browseForBrir();
	PRESENTATION_API void play();

private:
	void browseAndUpdateIfNotCancelled(
		std::vector<std::string> filters,
		std::function<void(std::string)>);
};
