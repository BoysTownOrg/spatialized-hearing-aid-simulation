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
	const auto result = view->browseForFile();
	if (!view->browseCancelled())
		view->setDslPrescriptionFilePath(result);
}

void Presenter::browseForAudio() {
	const auto result = view->browseForFile();
	if (!view->browseCancelled())
		view->setAudioFilePath(result);
}

void Presenter::browseForBrir() {
}
