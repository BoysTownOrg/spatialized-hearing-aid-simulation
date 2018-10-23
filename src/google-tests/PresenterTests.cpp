#include <presentation/Presenter.h>
#include <gtest/gtest.h>

class MockModel : public Model {};

class MockView : public View {
	std::string _dslPrescriptionFilePath{};
	std::string _audioFilePath{};
	std::string _browseFilePath{};
	Presenter *_presenter{};
	bool _runningEventLoop{};
	bool _browseCancelled{};
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
	void setDslPrescriptionFilePath(std::string p) override {
		_dslPrescriptionFilePath = p;
	}
	void setBrowseCancelled() {
		_browseCancelled = true;
	}
	void browseForDslPrescription() {
		_presenter->browseForDslPrescription();
	}
	std::string dslPrescriptionFilePath() const {
		return _dslPrescriptionFilePath;
	}
	void setBrowseFilePath(std::string p) {
		_browseFilePath = p;
	}
	virtual std::string browseForFile() override
	{
		return _browseFilePath;
	}
	virtual bool browseCancelled() override
	{
		return _browseCancelled;
	}
	void setAudioFilePath(std::string p) override {
		_audioFilePath = p;
	}
	void browseForAudio() {
		_presenter->browseForAudio();
	}
	std::string audioFilePath() const {
		return _audioFilePath;
	}
};

class PresenterFacade {
	Presenter presenter;
public:
	PresenterFacade(std::shared_ptr<View> view) :
		presenter{ std::make_shared<MockModel>(), std::move(view) } {}
	const Presenter *get() const {
		return &presenter;
	}
	void loop() {
		presenter.loop();
	}
};

class PresenterTestCase : public ::testing::TestCase {};

TEST(PresenterTestCase, constructorSetsItself) {
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	EXPECT_EQ(presenter.get(), view->presenter());
}

TEST(PresenterTestCase, loopRunsEventLoop) {
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	presenter.loop();
	EXPECT_TRUE(view->runningEventLoop());
}

TEST(
	PresenterTestCase, 
	cancellingBrowseForDslPrescriptionDoesNotChangeDslPrescriptionFilePath) 
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setDslPrescriptionFilePath("a");
	view->setBrowseCancelled();
	view->browseForDslPrescription();
	EXPECT_EQ("a", view->dslPrescriptionFilePath());
}

TEST(
	PresenterTestCase,
	browseForDslPrescriptionUpdatesDslPrescriptionFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForDslPrescription();
	EXPECT_EQ("a", view->dslPrescriptionFilePath());
}

TEST(
	PresenterTestCase,
	cancellingBrowseForAudioDoesNotChangeAudioFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setAudioFilePath("a");
	view->setBrowseCancelled();
	view->browseForAudio();
	EXPECT_EQ("a", view->audioFilePath());
}

TEST(
	PresenterTestCase,
	browseForAudioUpdatesAudioFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForAudio();
	EXPECT_EQ("a", view->audioFilePath());
}