#include "Presenter.h"

Presenter::Presenter(
	std::shared_ptr<Model> model, 
	std::shared_ptr<View> view
) :
	view{ std::move(view) }
{
	this->view->setPresenter(this);
}

void Presenter::loop()
{
	view->runEventLoop();
}

void Presenter::browseForDslPrescription()
{
}
