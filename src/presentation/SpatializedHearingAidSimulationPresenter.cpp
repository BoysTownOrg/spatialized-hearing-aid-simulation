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
	SpatializedHearingAidSimulationModel::PlayRequest request;
	request.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	request.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	request.brirFilePath = view->brirFilePath();
	request.audioFilePath = view->audioFilePath();
	try {
		request.level_dB_Spl = std::stod(view->level_dB_Spl());
	}
	catch (const std::invalid_argument &) {
		view->showErrorDialog("'" + view->level_dB_Spl() + "' is not a valid level.");
		return;
	}
	try {
		request.attack_ms = std::stod(view->attack_ms());
	}
	catch (const std::invalid_argument &) {
		view->showErrorDialog("'" + view->attack_ms() + "' is not a valid attack time.");
		return;
	}
	try {
		request.release_ms = std::stod(view->release_ms());
	}
	catch (const std::invalid_argument &) {
		view->showErrorDialog("'" + view->release_ms() + "' is not a valid release time.");
		return;
	}
	try {
		if (!onlyContainsDigits(view->windowSize()))
			throw std::invalid_argument{""};
		request.windowSize = std::stoi(view->windowSize());
	}
	catch (const std::invalid_argument &) {
		view->showErrorDialog("'" + view->windowSize() + "' is not a valid window size.");
		return;
	}
	try {
		if (!onlyContainsDigits(view->chunkSize()))
			throw std::invalid_argument{ "" };
		request.chunkSize = std::stoi(view->chunkSize());
	}
	catch (const std::invalid_argument &) {
		view->showErrorDialog("'" + view->chunkSize() + "' is not a valid chunk size.");
		return;
	}
	try {
		model->playRequest(request);
	}
	catch (const SpatializedHearingAidSimulationModel::RequestFailure &failure) {
		view->showErrorDialog(failure.what());
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
