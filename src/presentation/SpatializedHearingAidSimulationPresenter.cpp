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

void SpatializedHearingAidSimulationPresenter::play() {
	try {
		SpatializedHearingAidSimulationModel::PlayRequest request;
		request.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
		request.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
		request.brirFilePath = view->brirFilePath();
		request.audioFilePath = view->audioFilePath();
		request.level_dB_Spl = view->level_dB_Spl();
		request.attack_ms = view->attack_ms();
		request.release_ms = view->release_ms();
		request.windowSize = view->windowSize();
		request.chunkSize = view->chunkSize();
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
