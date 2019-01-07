#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

class ModelStub : public Model {
	std::vector<std::string> _audioDeviceDescriptions{};
	PlayRequest _request{};
	TestParameters _testParameters{};
	bool _testComplete{};
	bool _trialPlayed{};
public:
	const PlayRequest &request() const {
		return _request;
	}

	void play(PlayRequest request) override {
		_request = std::move(request);
	}
	const TestParameters &testParameters() const {
		return _testParameters;
	}

	void setAudioDeviceDescriptions(std::vector<std::string> d) {
		_audioDeviceDescriptions = std::move(d);
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return _audioDeviceDescriptions;
	}

	void initializeTest(TestParameters p) override {
		_testParameters = std::move(p);
	}

	void setTestIncomplete() {
		_testComplete = false;
	}

	bool trialPlayed() const {
		return _trialPlayed;
	}

	void playTrial() override {
		_trialPlayed = true;
	}
};

class ViewStub : public View {
	std::vector<std::string> _browseFilters{};
	std::vector<std::string> _audioDeviceMenuItems{};
	std::string _leftDslPrescriptionFilePath{};
	std::string _rightDslPrescriptionFilePath{};
	std::string _brirFilePath{};
	std::string _audioDevice{};
	std::string _browseFilePath{};
	std::string _level_dB_Spl{ "0" };
	std::string _attack_ms{ "0" };
	std::string _release_ms{ "0" };
	std::string _windowSize{ "0" };
	std::string _chunkSize{ "0" };
	std::string _errorMessage{};
	std::string _browseDirectory{};
	std::string _audioDirectory{};
	EventListener *_listener{};
	bool _runningEventLoop{};
	bool _browseCancelled{};
	bool _testSetupShown{};
	bool _testSetupHidden{};
	bool _testerViewShown{};
	bool _testerViewHidden{};
public:
	void subscribe(EventListener * listener) override {
		_listener = listener;
	}

	EventListener *listener() const {
		return _listener;
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
		_browseFilters = std::move(filters);
		return _browseFilePath;
	}

	void setBrowseFilePath(std::string p) {
		_browseFilePath = std::move(p);
	}

	bool browseCancelled() override {
		return _browseCancelled;
	}

	void setBrowseCancelled() {
		_browseCancelled = true;
	}

	void browseForLeftDslPrescription() {
		_listener->browseForLeftDslPrescription();
	}

	void setLeftDslPrescriptionFilePath(std::string p) override {
		_leftDslPrescriptionFilePath = std::move(p);
	}

	std::string leftDslPrescriptionFilePath() override {
		return _leftDslPrescriptionFilePath;
	}

	void browseForRightDslPrescription() {
		_listener->browseForRightDslPrescription();
	}

	std::string rightDslPrescriptionFilePath() override {
		return _rightDslPrescriptionFilePath;
	}

	void setRightDslPrescriptionFilePath(std::string p) override {
		_rightDslPrescriptionFilePath = std::move(p);
	}

	void browseForAudio() {
		_listener->browseForAudio();
	}

	void browseForBrir() {
		_listener->browseForBrir();
	}

	void setBrirFilePath(std::string p) override {
		_brirFilePath = std::move(p);
	}

	std::string brirFilePath() override {
		return _brirFilePath;
	}

	std::vector<std::string> browseFilters() const {
		return _browseFilters;
	}

	void setLevel_dB_Spl(std::string level) {
		_level_dB_Spl = std::move(level);
	}

	std::string level_dB_Spl() override {
		return _level_dB_Spl;
	}

	void setAttack_ms(std::string a) {
		_attack_ms = std::move(a);
	}

	std::string attack_ms() override {
		return _attack_ms;
	}

	void setRelease_ms(std::string r) {
		_release_ms = std::move(r);
	}

	std::string release_ms() override {
		return _release_ms;
	}

	void setWindowSize(std::string s) {
		_windowSize = std::move(s);
	}

	std::string windowSize() override {
		return _windowSize;
	}

	void setChunkSize(std::string s) {
		_chunkSize = std::move(s);
	}

	std::string chunkSize() override {
		return _chunkSize;
	}

	std::string errorMessage() const {
		return _errorMessage;
	}

	void showErrorDialog(std::string message) override {
		_errorMessage = std::move(message);
	}

	std::vector<std::string> audioDeviceMenuItems() const {
		return _audioDeviceMenuItems;
	}

	void populateAudioDeviceMenu(std::vector<std::string> d) override {
		_audioDeviceMenuItems = std::move(d);
	}

	void setAudioDevice(std::string s) {
		_audioDevice = std::move(s);
	}

	std::string audioDevice() override {
		return _audioDevice;
	}

	void setBrowseDirectory(std::string d) {
		_browseDirectory = std::move(d);
	}

	std::string audioDirectory() override {
		return _audioDirectory;
	}

	std::string browseForDirectory() override {
		return _browseDirectory;
	}

	void setAudioDirectory(std::string d) override {
		_audioDirectory = std::move(d);
	}

	void newTest() {
		_listener->newTest();
	}

	bool testSetupShown() const {
		return _testSetupShown;
	}

	void showTestSetup() override {
		_testSetupShown = true;
	}

	void confirmTestSetup() {
		_listener->confirmTestSetup();
	}

	bool testSetupHidden() const {
		return _testSetupHidden;
	}

	void hideTestSetup() override {
		_testSetupHidden = true;
	}

	bool testerViewShown() const {
		return _testerViewShown;
	}

	void showTesterView() override {
		_testerViewShown = true;
	}

	void playTrial() {
		_listener->playTrial();
	}

	bool testerViewHidden() const {
		return _testerViewHidden;
	}
};

class PresenterTests : public ::testing::Test {
protected:
	std::shared_ptr<ModelStub> model = std::make_shared<ModelStub>();
	std::shared_ptr<ViewStub> view = std::make_shared<ViewStub>();
	Presenter presenter;

	PresenterTests() : presenter{ model, view } {}

	void setInvalidChunkSize() {
		view->setChunkSize("?");
	}

	void setInvalidWindowSize() {
		view->setWindowSize("a");
	}

	void setInvalidReleaseTime() {
		view->setRelease_ms("b");
	}

	void setInvalidAttackTime() {
		view->setAttack_ms("c");
	}

	void setInvalidLevel() {
		view->setLevel_dB_Spl("d");
	}

	void confirmTestSetupDoesNotHideSetupView() {
		view->confirmTestSetup();
		EXPECT_FALSE(view->testSetupHidden());
	}

	void confirmTestSetupDoesNotShowTesterView() {
		view->confirmTestSetup();
		EXPECT_FALSE(view->testerViewShown());
	}

	void confirmTestSetupWithChunkSizeShowsErrorMessage(std::string s) {
		view->setChunkSize(s);
		confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid chunk size.");
	}

	void confirmTestSetupShowsErrorMessage(std::string s) {
		view->confirmTestSetup();
		assertEqual(std::move(s), view->errorMessage());
	}

	void confirmTestSetupWithWindowSizeShowsErrorMessage(std::string s) {
		view->setWindowSize(s);
		confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid window size.");
	}

	void confirmTestSetupWithAttackTimeShowsErrorMessage(std::string s) {
		view->setAttack_ms(s);
		confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid attack time.");
	}

	void confirmTestSetupWithReleaseTimeShowsErrorMessage(std::string s) {
		view->setRelease_ms(s);
		confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid release time.");
	}

	void confirmTestSetupWithLevelShowsErrorMessage(std::string s) {
		view->setLevel_dB_Spl(s);
		confirmTestSetupShowsErrorMessage("'" + s + "' is not a valid level.");
	}
};

TEST_F(PresenterTests, subscribesToViewEvents) {
	EXPECT_EQ(&presenter, view->listener());
}

TEST_F(PresenterTests, runRunsEventLoop) {
	presenter.run();
	EXPECT_TRUE(view->runningEventLoop());
}

TEST_F(PresenterTests, newTestShowsTestSetupView) {
    view->newTest();
    EXPECT_TRUE(view->testSetupShown());
}

TEST_F(PresenterTests, confirmTestSetupHidesTestSetupView) {
    view->confirmTestSetup();
    EXPECT_TRUE(view->testSetupHidden());
}

TEST_F(
	PresenterTests, 
	cancellingBrowseForDslPrescriptionDoesNotChangeDslPrescriptionFilePath
) {
	view->setLeftDslPrescriptionFilePath("a");
	view->setRightDslPrescriptionFilePath("b");
	view->setBrowseCancelled();
	view->browseForLeftDslPrescription();
	assertEqual("a", view->leftDslPrescriptionFilePath());
	view->browseForRightDslPrescription();
	assertEqual("b", view->rightDslPrescriptionFilePath());
}

TEST_F(
	PresenterTests,
	cancellingBrowseForAudioDirectoryNotChangeAudioDirectory
) {
	view->setAudioDirectory("a");
	view->setBrowseCancelled();
	view->browseForAudio();
	assertEqual("a", view->audioDirectory());
}

TEST_F(
	PresenterTests,
	cancellingBrowseForBrirDoesNotChangeBrirFilePath
) {
	view->setBrirFilePath("a");
	view->setBrowseCancelled();
	view->browseForBrir();
	assertEqual("a", view->brirFilePath());
}

TEST_F(
	PresenterTests,
	browseForDslPrescriptionUpdatesDslPrescriptionFilePath
) {
	view->setBrowseFilePath("a");
	view->browseForLeftDslPrescription();
	assertEqual("a", view->leftDslPrescriptionFilePath());
	view->setBrowseFilePath("b");
	view->browseForRightDslPrescription();
	assertEqual("b", view->rightDslPrescriptionFilePath());
}

TEST_F(
	PresenterTests,
	browseForAudioDirectoryUpdatesAudioDirectory
) {
	view->setBrowseDirectory("a");
	view->browseForAudio();
	assertEqual("a", view->audioDirectory());
}

TEST_F(
	PresenterTests,
	browseForBrirUpdatesBrirFilePath
) {
	view->setBrowseFilePath("a");
	view->browseForBrir();
	assertEqual("a", view->brirFilePath());
}

TEST_F(
	PresenterTests,
	browseForBrirFiltersWavFiles
) {
	view->browseForBrir();
	assertEqual({ "*.wav" }, view->browseFilters());
}

TEST_F(
	PresenterTests,
	confirmTestSetupPassesParametersToModel
) {
	view->setLeftDslPrescriptionFilePath("a");
	view->setRightDslPrescriptionFilePath("b");
	view->setAudioDirectory("c");
	view->setBrirFilePath("d");
	view->setAudioDevice("e");
	view->setLevel_dB_Spl("1.1");
	view->setAttack_ms("2.2");
	view->setRelease_ms("3.3");
	view->setWindowSize("4");
	view->setChunkSize("5");
	view->confirmTestSetup();
	assertEqual("a", model->testParameters().leftDslPrescriptionFilePath);
	assertEqual("b", model->testParameters().rightDslPrescriptionFilePath);
	assertEqual("c", model->testParameters().audioDirectory);
	assertEqual("d", model->testParameters().brirFilePath);
	assertEqual("e", model->testParameters().audioDevice);
	EXPECT_EQ(1.1, model->testParameters().level_dB_Spl);
	EXPECT_EQ(2.2, model->testParameters().attack_ms);
	EXPECT_EQ(3.3, model->testParameters().release_ms);
	EXPECT_EQ(4, model->testParameters().windowSize);
	EXPECT_EQ(5, model->testParameters().chunkSize);
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotHideSetupView) {
	setInvalidChunkSize();
	confirmTestSetupDoesNotHideSetupView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotHideSetupView) {
	setInvalidWindowSize();
	confirmTestSetupDoesNotHideSetupView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotHideSetupView) {
	setInvalidReleaseTime();
	confirmTestSetupDoesNotHideSetupView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotHideSetupView) {
	setInvalidAttackTime();
	confirmTestSetupDoesNotHideSetupView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidLevelDoesNotHideSetupView) {
	setInvalidLevel();
	confirmTestSetupDoesNotHideSetupView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeDoesNotShowTesterView) {
	setInvalidChunkSize();
	confirmTestSetupDoesNotShowTesterView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeDoesNotShowTesterView) {
	setInvalidWindowSize();
	confirmTestSetupDoesNotShowTesterView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeDoesNotShowTesterView) {
	setInvalidReleaseTime();
	confirmTestSetupDoesNotShowTesterView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeDoesNotShowTesterView) {
	setInvalidAttackTime();
	confirmTestSetupDoesNotShowTesterView();
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidLevelDoesNotShowTesterView) {
	setInvalidLevel();
	confirmTestSetupDoesNotShowTesterView();
}

TEST_F(PresenterTests, confirmTestSetupShowsTesterView) {
	view->confirmTestSetup();
    EXPECT_TRUE(view->testerViewShown());
}

TEST_F(PresenterTests, playingTrialDoesNotHideViewWhileTestInProgress) {
    model->setTestIncomplete();
    view->playTrial();
    EXPECT_FALSE(view->testerViewHidden());
}

TEST_F(PresenterTests, playingTrialPlaysTrial) {
    view->playTrial();
    EXPECT_TRUE(model->trialPlayed());
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidChunkSizeShowsErrorMessage) {
	for (auto s : std::vector<std::string>{ "a", "0.1", "-1" })
		confirmTestSetupWithChunkSizeShowsErrorMessage(s);
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidWindowSizeShowsErrorMessage) {
	for (auto s : std::vector<std::string>{ "a", "0.1", "-1" })
		confirmTestSetupWithWindowSizeShowsErrorMessage(s);
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidAttackTimeShowsErrorMessage) {
	confirmTestSetupWithAttackTimeShowsErrorMessage("a");
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidReleaseTimeShowsErrorMessage) {
	confirmTestSetupWithReleaseTimeShowsErrorMessage("b");
}

TEST_F(PresenterTests, confirmTestSetupWithInvalidLevelShowsErrorMessage) {
	confirmTestSetupWithLevelShowsErrorMessage("c");
}

TEST(PresenterAudioDeviceTest, constructorPopulatesAudioDeviceMenu) {
	const auto view = std::make_shared<ViewStub>();
	const auto model = std::make_shared<ModelStub>();
	model->setAudioDeviceDescriptions({ "a", "b", "c" });
	Presenter presenter{ model, view };
	assertEqual({ "a", "b", "c" }, view->audioDeviceMenuItems());
}

class InitializationFailingModel : public Model {
	std::string message{};
public:
	void setErrorMessage(std::string s) {
		message = std::move(s);
	}

	void initializeTest(TestParameters) override {
		throw TestInitializationFailure{ message };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	void play(PlayRequest) override {}
	void playTrial() override {}
};

class PresenterWithInitializationFailingModel : public ::testing::Test {
protected:
	std::shared_ptr<InitializationFailingModel> model = 
		std::make_shared<InitializationFailingModel>();
	std::shared_ptr<ViewStub> view = std::make_shared<ViewStub>();
	Presenter presenter;

	PresenterWithInitializationFailingModel() : presenter{ model, view } {}
};

TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupShowsErrorMessage) {
	model->setErrorMessage("error.");
	view->confirmTestSetup();
	assertEqual("error.", view->errorMessage());
}

TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupDoesNotShowTesterView) {
	view->confirmTestSetup();
	EXPECT_FALSE(view->testerViewShown());
}

TEST_F(PresenterWithInitializationFailingModel, confirmTestSetupDoesNotHideSetupView) {
	view->confirmTestSetup();
	EXPECT_FALSE(view->testSetupHidden());
}
