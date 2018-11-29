#include "SpatializedHearingAidSimulationPresenter.h"

SpatializedHearingAidSimulationPresenter::SpatializedHearingAidSimulationPresenter(
	std::shared_ptr<SpatializedHearingAidSimulationModel> model, 
	std::shared_ptr<SpatializedHearingAidSimulationView> view
) :
	model{ std::move(model) },
	view{ std::move(view) }
{
	this->view->setPresenter(this);
}

void SpatializedHearingAidSimulationPresenter::loop() {
	view->runEventLoop();
}

void SpatializedHearingAidSimulationPresenter::browseForLeftDslPrescription() {
	browseAndUpdateIfNotCancelled(
		{},
		[=](std::string p) { this->view->setLeftDslPrescriptionFilePath(p); });
}

void SpatializedHearingAidSimulationPresenter::browseForRightDslPrescription() {
	browseAndUpdateIfNotCancelled(
		{},
		[=](std::string p) { this->view->setRightDslPrescriptionFilePath(p); });
}

void SpatializedHearingAidSimulationPresenter::browseForAudio() {
	browseAndUpdateIfNotCancelled(
		{ "*.wav" },
		[=](std::string p) { this->view->setAudioFilePath(p); });
}

void SpatializedHearingAidSimulationPresenter::browseForBrir() {
	browseAndUpdateIfNotCancelled(
		{ "*.mat" },
		[=](std::string p) { this->view->setBrirFilePath(p); });
}

static bool onlyContainsDigits(const std::string s) {
	return s.find_first_not_of("0123456789") == std::string::npos;
}

void SpatializedHearingAidSimulationPresenter::play() {
	try {
		SpatializedHearingAidSimulationModel::PlayRequest request;
		request.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
		request.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
		request.brirFilePath = view->brirFilePath();
		request.audioFilePath = view->audioFilePath();
		request.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
		request.attack_ms = convertToDouble(view->attack_ms(), "attack time");
		request.release_ms = convertToDouble(view->release_ms(), "release time");
		request.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
		request.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
		model->playRequest(request);
	}
	catch (const BadInput &e) {
		view->showErrorDialog(e.what());
	}
	catch (const SpatializedHearingAidSimulationModel::RequestFailure &failure) {
		view->showErrorDialog(failure.what());
	}
}

double SpatializedHearingAidSimulationPresenter::convertToDouble(
	std::string x, 
	std::string identifier)
{
	try {
		return std::stod(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput("'" + x + "' is not a valid " + identifier + ".");
	}
}

int SpatializedHearingAidSimulationPresenter::convertToPositiveInteger(std::string x, std::string identifier)
{
	try {
		if (!onlyContainsDigits(x))
			throw std::invalid_argument{ "" };
		return std::stoi(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput("'" + x + "' is not a valid " + identifier + ".");
	}
}

void SpatializedHearingAidSimulationPresenter::browseAndUpdateIfNotCancelled(
	std::vector<std::string> filters, 
	std::function<void(std::string)> update) 
{
	const auto filePath = view->browseForFile(filters);
	if (!view->browseCancelled())
		update(filePath);
}
