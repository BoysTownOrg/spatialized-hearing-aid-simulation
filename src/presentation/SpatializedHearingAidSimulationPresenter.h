#pragma once

#include "presentation-exports.h"
#include "SpatializedHearingAidSimulationView.h"
#include "SpatializedHearingAidSimulationModel.h"
#include <common-includes/RuntimeError.h>
#include <memory>
#include <functional>

class SpatializedHearingAidSimulationPresenter {
	std::shared_ptr<SpatializedHearingAidSimulationModel> model;
	std::shared_ptr<SpatializedHearingAidSimulationView> view;
public:
	PRESENTATION_API SpatializedHearingAidSimulationPresenter(
		std::shared_ptr<SpatializedHearingAidSimulationModel> model, 
		std::shared_ptr<SpatializedHearingAidSimulationView> view);
	PRESENTATION_API void loop();
	PRESENTATION_API void browseForLeftDslPrescription();
	PRESENTATION_API void browseForRightDslPrescription();
	PRESENTATION_API void browseForAudio();
	PRESENTATION_API void browseForBrir();
	PRESENTATION_API void play();

private:
	RUNTIME_ERROR(BadInput);
	void browseAndUpdateIfNotCancelled(
		std::vector<std::string> filters,
		std::function<void(std::string)>);
	double convertToDouble(std::string x, std::string identifier);
	int convertToPositiveInteger(std::string x, std::string identifier);
};
