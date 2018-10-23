#include <presentation/Presenter.h>
#include <gtest/gtest.h>

class MockModel : public Model {};

class MockView : public View {
	Presenter *_presenter{};
	bool _runningEventLoop{};
public:
	Presenter *presenter() const {
		return _presenter;
	}
	void setPresenter(Presenter *p) override {
		_presenter = p;
	}
	bool runningEventLoop() const {
		return _runningEventLoop;
	}
	void runEventLoop() override {
		_runningEventLoop = true;
	}
};

class PresenterTestCase : public ::testing::TestCase {};

TEST(PresenterTestCase, constructorSetsItself) {
	const auto view = std::make_shared<MockView>();
	Presenter presenter{ std::make_shared<MockModel>(), view };
	EXPECT_EQ(&presenter, view->presenter());
}

TEST(PresenterTestCase, loopRunsEventLoop) {
	const auto view = std::make_shared<MockView>();
	Presenter presenter{ std::make_shared<MockModel>(), view };
	presenter.loop();
	EXPECT_TRUE(view->runningEventLoop());
}