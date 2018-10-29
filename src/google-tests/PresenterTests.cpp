#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

class MockModel : public Model {
	std::string _leftDslPrescriptionFilePath{};
	std::string _audioFilePath{};
	std::string _brirFilePath{};
public:
	std::string leftDslPrescriptionFilePath() const {
		return _leftDslPrescriptionFilePath;
	}
	std::string audioFilePath() const {
		return _audioFilePath;
	}
	std::string brirFilePath() const {
		return _brirFilePath;
	}
	void playRequest(PlayRequest request) override {
		_leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
		_audioFilePath = request.audioFilePath;
		_brirFilePath = request.brirFilePath;
	}
};

class MockView : public View {
	std::vector<std::string> _browseFilters{};
	std::string _leftDslPrescriptionFilePath{};
	std::string _rightDslPrescriptionFilePath{};
	std::string _audioFilePath{};
	std::string _brirFilePath{};
	std::string _browseFilePath{};
	Presenter *_presenter{};
	bool _runningEventLoop{};
	bool _browseCancelled{};
public:
	void setPresenter(Presenter *p) override {
		_presenter = p;
	}
	Presenter *presenter() const {
		return _presenter;
	}
	void runEventLoop() override {
		_runningEventLoop = true;
	}
	bool runningEventLoop() const {
		return _runningEventLoop;
	}
	virtual std::string browseForFile(
		std::vector<std::string> filters) override 
	{
		_browseFilters = filters;
		return _browseFilePath;
	}
	void setBrowseFilePath(std::string p) {
		_browseFilePath = p;
	}
	virtual bool browseCancelled() override {
		return _browseCancelled;
	}
	void setBrowseCancelled() {
		_browseCancelled = true;
	}
	void browseForLeftDslPrescription() {
		_presenter->browseForLeftDslPrescription();
	}
	void setLeftDslPrescriptionFilePath(std::string p) override {
		_leftDslPrescriptionFilePath = p;
	}
	std::string leftDslPrescriptionFilePath() const override {
		return _leftDslPrescriptionFilePath;
	}
	void browseForRightDslPrescription() {
		_presenter->browseForRightDslPrescription();
	}
	std::string rightDslPrescriptionFilePath() const {
		return _rightDslPrescriptionFilePath;
	}
	void setRightDslPrescriptionFilePath(std::string p) override {
		_rightDslPrescriptionFilePath = p;
	}
	void browseForAudio() {
		_presenter->browseForAudio();
	}
	void setAudioFilePath(std::string p) override {
		_audioFilePath = p;
	}
	std::string audioFilePath() const override {
		return _audioFilePath;
	}
	void browseForBrir() {
		_presenter->browseForBrir();
	}
	void setBrirFilePath(std::string p) override {
		_brirFilePath = p;
	}
	std::string brirFilePath() const override {
		return _brirFilePath;
	}
	std::vector<std::string> browseFilters() const {
		return _browseFilters;
	}
	void play() {
		_presenter->play();
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
	view->setLeftDslPrescriptionFilePath("a");
	view->setRightDslPrescriptionFilePath("b");
	view->setBrowseCancelled();
	view->browseForLeftDslPrescription();
	EXPECT_EQ("a", view->leftDslPrescriptionFilePath());
	view->browseForRightDslPrescription();
	EXPECT_EQ("b", view->rightDslPrescriptionFilePath());
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
	cancellingBrowseForBrirDoesNotChangeBrirFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrirFilePath("a");
	view->setBrowseCancelled();
	view->browseForBrir();
	EXPECT_EQ("a", view->brirFilePath());
}

TEST(
	PresenterTestCase,
	browseForDslPrescriptionUpdatesDslPrescriptionFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForLeftDslPrescription();
	EXPECT_EQ("a", view->leftDslPrescriptionFilePath());
	view->setBrowseFilePath("b");
	view->browseForRightDslPrescription();
	EXPECT_EQ("b", view->rightDslPrescriptionFilePath());
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

TEST(
	PresenterTestCase,
	browseForBrirUpdatesBrirFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForBrir();
	EXPECT_EQ("a", view->brirFilePath());
}

TEST(
	PresenterTestCase,
	browseForAudioFiltersWavFiles)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->browseForAudio();
	assertEqual({ "*.wav" }, view->browseFilters());
}

TEST(
	PresenterTestCase,
	browseForBrirFiltersMatFiles)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->browseForBrir();
	assertEqual({ "*.mat" }, view->browseFilters());
}

TEST(
	PresenterTestCase,
	playPassesParametersToPlayRequest)
{
	const auto view = std::make_shared<MockView>();
	const auto model = std::make_shared<MockModel>();
	Presenter presenter{ model, view };
	view->setLeftDslPrescriptionFilePath("a");
	view->setAudioFilePath("b");
	view->setBrirFilePath("c");
	view->play();
	EXPECT_EQ("a", model->leftDslPrescriptionFilePath());
	EXPECT_EQ("b", model->audioFilePath());
	EXPECT_EQ("c", model->brirFilePath());
}