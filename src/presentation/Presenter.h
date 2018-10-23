#pragma once

#ifdef PRESENTATION_EXPORTS
	#define PRESENTATION_API __declspec(dllexport)
#else
	#define PRESENTATION_API __declspec(dllimport)
#endif

class Model {};

#include "View.h"
#include <memory>

class Presenter {
public:
	PRESENTATION_API Presenter(
		std::shared_ptr<Model> model, 
		std::shared_ptr<View> view);
};
