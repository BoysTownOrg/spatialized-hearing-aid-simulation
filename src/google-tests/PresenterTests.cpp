#include "assert-utility.h"
#include <presentation/SpatializedHearingAidSimulationPresenter.h>
#include <gtest/gtest.h>

class MockModel : public SpatializedHearingAidSimulationModel {
	PlayRequest _request{};
public:
	const PlayRequest &request() const {
		return _request;
	}

	void playRequest(PlayRequest request) override {
		_request = request;
	}
};

class MockView : public SpatializedHearingAidSimulationView {
	std::vector<std::string> _browseFilters{};
	std::string _leftDslPrescriptionFilePath{};
	std::string _rightDslPrescriptionFilePath{};
	std::string _audioFilePath{};
	std::string _brirFilePath{};
	std::string _browseFilePath{};
	std::string _level_dB_Spl{ "0" };
	std::string _attack_ms{ "0" };
	std::string _release_ms{ "0" };
	std::string _windowSize{ "0" };
	std::string _chunkSize{ "0" };
	std::string _errorMessage{};
	SpatializedHearingAidSimulationPresenter *_presenter{};
	bool _runningEventLoop{};
	bool _browseCancelled{};
public:
	void setPresenter(SpatializedHearingAidSimulationPresenter *p) override {
		_presenter = p;
	}

	SpatializedHearingAidSimulationPresenter *presenter() const {
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

	bool browseCancelled() override {
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

	std::string rightDslPrescriptionFilePath() const override {
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

	void setLevel_dB_Spl(std::string level) {
		_level_dB_Spl = level;
	}

	std::string level_dB_Spl() const override {
		return _level_dB_Spl;
	}

	void setAttack_ms(std::string a) {
		_attack_ms = a;
	}

	std::string attack_ms() const override {
		return _attack_ms;
	}

	void setRelease_ms(std::string r) {
		_release_ms = r;
	}

	std::string release_ms() const override {
		return _release_ms;
	}

	void setWindowSize(std::string s) {
		_windowSize = s;
	}

	std::string windowSize() const override {
		return _windowSize;
	}

	void setChunkSize(std::string s) {
		_chunkSize = s;
	}

	std::string chunkSize() const override {
		return _chunkSize;
	}

	void play() {
		_presenter->play();
	}

	std::string errorMessage() const {
		return _errorMessage;
	}

	void showErrorDialog(std::string message) override {
		_errorMessage = message;
	}
};

class PresenterFacade {
	SpatializedHearingAidSimulationPresenter presenter;
public:
	PresenterFacade(std::shared_ptr<SpatializedHearingAidSimulationView> view) :
		presenter{ std::make_shared<MockModel>(), std::move(view) } {}

	const SpatializedHearingAidSimulationPresenter *get() const {
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

TEST(PresenterTestCase, constructorPopulatesAudioDeviceMenu) {
	const auto view = std::make_shared<MockView>();
	const auto model = std::make_shared<MockModel>();
	model->setAudioDeviceDescriptions({ "a", "b", "c" });
	SpatializedHearingAidSimulationPresenter presenter{ model, view };
	assertEqual({ "a", "b", "c" }, view->audioDeviceMenuItems());
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
	assertEqual("a", view->leftDslPrescriptionFilePath());
	view->browseForRightDslPrescription();
	assertEqual("b", view->rightDslPrescriptionFilePath());
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
	assertEqual("a", view->audioFilePath());
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
	assertEqual("a", view->brirFilePath());
}

TEST(
	PresenterTestCase,
	browseForDslPrescriptionUpdatesDslPrescriptionFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForLeftDslPrescription();
	assertEqual("a", view->leftDslPrescriptionFilePath());
	view->setBrowseFilePath("b");
	view->browseForRightDslPrescription();
	assertEqual("b", view->rightDslPrescriptionFilePath());
}

TEST(
	PresenterTestCase,
	browseForAudioUpdatesAudioFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForAudio();
	assertEqual("a", view->audioFilePath());
}

TEST(
	PresenterTestCase,
	browseForBrirUpdatesBrirFilePath)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	view->setBrowseFilePath("a");
	view->browseForBrir();
	assertEqual("a", view->brirFilePath());
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
	SpatializedHearingAidSimulationPresenter presenter{ model, view };
	view->setLeftDslPrescriptionFilePath("a");
	view->setRightDslPrescriptionFilePath("b");
	view->setAudioFilePath("c");
	view->setBrirFilePath("d");
	view->setLevel_dB_Spl("1.1");
	view->setAttack_ms("2.2");
	view->setRelease_ms("3.3");
	view->setWindowSize("4");
	view->setChunkSize("5");
	view->play();
	assertEqual("a", model->request().leftDslPrescriptionFilePath);
	assertEqual("b", model->request().rightDslPrescriptionFilePath);
	assertEqual("c", model->request().audioFilePath);
	assertEqual("d", model->request().brirFilePath);
	EXPECT_EQ(1.1, model->request().level_dB_Spl);
	EXPECT_EQ(2.2, model->request().attack_ms);
	EXPECT_EQ(3.3, model->request().release_ms);
	EXPECT_EQ(4, model->request().windowSize);
	EXPECT_EQ(5, model->request().chunkSize);
}

class ErrorModel : public SpatializedHearingAidSimulationModel {
	std::string message;
public:
	explicit ErrorModel(std::string message) :
		message{std::move(message)} {}

	void playRequest(PlayRequest) override {
		throw RequestFailure{ message };
	}
};

TEST(PresenterTestCase, requestFailureShowsErrorMessage) {
	const auto view = std::make_shared<MockView>();
	const auto model = std::make_shared<ErrorModel>("error.");
	SpatializedHearingAidSimulationPresenter presenter{ model, view };
	view->play();
	assertEqual("error.", view->errorMessage());
}

static void expectViewSettingYieldsErrorMessageOnPlay(
	std::function<void(MockView &)> transformation,
	std::string message)
{
	const auto view = std::make_shared<MockView>();
	PresenterFacade presenter{ view };
	transformation(*view);
	view->play();
	assertEqual(message, view->errorMessage());
}

TEST(PresenterTestCase, nonFloatsThrowRequestFailures) {
	expectViewSettingYieldsErrorMessageOnPlay(
		[](MockView & view) {
			view.setLevel_dB_Spl("a");
		},
		"'a' is not a valid level.");
	expectViewSettingYieldsErrorMessageOnPlay(
		[](MockView & view) {
			view.setAttack_ms("b");
		},
		"'b' is not a valid attack time.");
	expectViewSettingYieldsErrorMessageOnPlay(
		[](MockView & view) {
			view.setRelease_ms("c");
		},
		"'c' is not a valid release time.");
}

static void expectBadWindowSize(std::string size) {
	expectViewSettingYieldsErrorMessageOnPlay(
		[=](MockView & view) {
			view.setWindowSize(size);
		},
		"'" + size + "' is not a valid window size.");
}

static void expectBadChunkSize(std::string size) {
	expectViewSettingYieldsErrorMessageOnPlay(
		[=](MockView & view) {
			view.setChunkSize(size);
		},
		"'" + size + "' is not a valid chunk size.");
}

TEST(PresenterTestCase, nonPositiveIntegersThrowRequestFailures) {
	for (const auto s : std::vector<std::string>{ "a", "0.1", "-1" }) {
		expectBadWindowSize(s);
		expectBadChunkSize(s);
	}
}
