#include "Presenter.h"

Presenter::Presenter(std::shared_ptr<Model> model, std::shared_ptr<View> view) {
	view->setPresenter(this);
}
