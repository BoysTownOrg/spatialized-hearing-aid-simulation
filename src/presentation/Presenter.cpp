#include "Presenter.h"

Presenter::Presenter(
	std::shared_ptr<Model> model, 
	std::shared_ptr<View> view
) :
	view{ std::move(view) }
{
	this->view->setPresenter(this);
}

void Presenter::loop() {
	view->runEventLoop();
}

void Presenter::browseForDslPrescription() {
	browseAndUpdateIfNotCancelled(
		[=](std::string p) { this->view->setDslPrescriptionFilePath(p); });
}

void Presenter::browseForAudio() {
	browseAndUpdateIfNotCancelled(
		[=](std::string p) { this->view->setAudioFilePath(p); });
}

void Presenter::browseForBrir() {
	browseAndUpdateIfNotCancelled(
		[=](std::string p) { this->view->setBrirFilePath(p); });
}

void Presenter::browseAndUpdateIfNotCancelled(std::function<void(std::string)> update) {
	const auto filePath = view->browseForFile();
	if (!view->browseCancelled())
		update(filePath);
}
