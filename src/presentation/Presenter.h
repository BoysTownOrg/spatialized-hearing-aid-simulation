#pragma once

#ifdef PRESENTATION_EXPORTS
	#define PRESENTATION_API __declspec(dllexport)
#else
	#define PRESENTATION_API __declspec(dllimport)
#endif

class Model {};

#include "View.h"
#include <memory>
#include <functional>

class Presenter {
	std::shared_ptr<View> view;
public:
	PRESENTATION_API Presenter(
		std::shared_ptr<Model> model, 
		std::shared_ptr<View> view);
	PRESENTATION_API void loop();
	PRESENTATION_API void browseForDslPrescription();
	PRESENTATION_API void browseForAudio();
	PRESENTATION_API void browseForBrir();

private:
	void browseAndUpdateIfNotCancelled(std::function<void(std::string)>);
};
