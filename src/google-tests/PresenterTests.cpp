#define INTERFACE_OPERATIONS(class_name) \
    virtual ~class_name() = default;\
    class_name() = default;\
    class_name(const class_name&) = delete;\
    class_name& operator=(const class_name&) = delete;\
    class_name(class_name&&) = delete;\
    class_name& operator=(class_name&&) = delete;

class Model {};

class Presenter;

class View {
public:
	INTERFACE_OPERATIONS(View);
	virtual void setPresenter(Presenter *) = 0;
};

#include <memory>

class Presenter {
public:
	Presenter(std::shared_ptr<Model> model, std::shared_ptr<View> view) {
		view->setPresenter(this);
	}
};

#include <gtest/gtest.h>

class MockModel : public Model {};

class MockView : public View {
	Presenter *_presenter{};
public:
	Presenter *presenter() const {
		return _presenter;
	}
	void setPresenter(Presenter *p) override {
		_presenter = p;
	}
};

class PresenterTestCase : public ::testing::TestCase {};

TEST(PresenterTestCase, constructorSetsItself) {
	const auto view = std::make_shared<MockView>();
	Presenter presenter{ std::make_shared<MockModel>(), view };
	EXPECT_EQ(&presenter, view->presenter());
}