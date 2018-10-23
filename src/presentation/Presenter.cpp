#include "Presenter.h"

Presenter::Presenter(
	std::shared_ptr<Model> model, 
	std::shared_ptr<View> view
) :
	model{ std::move(model) },
	view{ std::move(view) }
{
	this->view->setPresenter(this);
}

void Presenter::loop() {
	view->runEventLoop();
}

void Presenter::browseForDslPrescription() {
	browseAndUpdateIfNotCancelled(
		{},
		[=](std::string p) { this->view->setDslPrescriptionFilePath(p); });
}

void Presenter::browseForAudio() {
	browseAndUpdateIfNotCancelled(
		{ "*.wav" },
		[=](std::string p) { this->view->setAudioFilePath(p); });
}

void Presenter::browseForBrir() {
	browseAndUpdateIfNotCancelled(
		{ "*.mat" },
		[=](std::string p) { this->view->setBrirFilePath(p); });
}

void Presenter::play() {
	model->playRequest(
		{
			view->dslPrescriptionFilePath(),
			view->audioFilePath(),
			view->brirFilePath()
		}
	);
}

void Presenter::browseAndUpdateIfNotCancelled(
	std::vector<std::string> filters, 
	std::function<void(std::string)> update) 
{
	const auto filePath = view->browseForFile(filters);
	if (!view->browseCancelled())
		update(filePath);
}
