#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

class ModelStub : public Model {
	std::vector<std::string> _audioDeviceDescriptions{};
	PlayRequest _request{};
public:
	const PlayRequest &request() const {
		return _request;
	}

	void play(PlayRequest request) override {
		_request = std::move(request);
	}

	void setAudioDeviceDescriptions(std::vector<std::string> d) {
		_audioDeviceDescriptions = std::move(d);
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return _audioDeviceDescriptions;
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

	void play() {
		_listener->play();
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
};

class PresenterTests : public ::testing::Test {
protected:
	std::shared_ptr<ModelStub> model = std::make_shared<ModelStub>();
	std::shared_ptr<ViewStub> view = std::make_shared<ViewStub>();
	Presenter presenter;

	PresenterTests() : presenter{ model, view } {}
};

TEST_F(PresenterTests, constructorSetsItself) {
	EXPECT_EQ(&presenter, view->listener());
}

TEST_F(PresenterTests, loopRunsEventLoop) {
	presenter.loop();
	EXPECT_TRUE(view->runningEventLoop());
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
	playPassesParametersToPlayRequest
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
	view->play();
	assertEqual("a", model->request().leftDslPrescriptionFilePath);
	assertEqual("b", model->request().rightDslPrescriptionFilePath);
	assertEqual("c", model->request().audioDirectory);
	assertEqual("d", model->request().brirFilePath);
	assertEqual("e", model->request().audioDevice);
	EXPECT_EQ(1.1, model->request().level_dB_Spl);
	EXPECT_EQ(2.2, model->request().attack_ms);
	EXPECT_EQ(3.3, model->request().release_ms);
	EXPECT_EQ(4, model->request().windowSize);
	EXPECT_EQ(5, model->request().chunkSize);
}

TEST(PresenterAudioDeviceTest, constructorPopulatesAudioDeviceMenu) {
	const auto view = std::make_shared<ViewStub>();
	const auto model = std::make_shared<ModelStub>();
	model->setAudioDeviceDescriptions({ "a", "b", "c" });
	Presenter presenter{ model, view };
	assertEqual({ "a", "b", "c" }, view->audioDeviceMenuItems());
}

class ErrorModel : public Model {
	std::string message;
public:
	explicit ErrorModel(std::string message) :
		message{std::move(message)} {}

	void play(PlayRequest) override {
		throw RequestFailure{ message };
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return {};
	}
};

TEST(PresenterErrorTests, requestFailureShowsErrorMessage) {
	const auto view = std::make_shared<ViewStub>();
	const auto model = std::make_shared<ErrorModel>("error.");
	Presenter presenter{ model, view };
	view->play();
	assertEqual("error.", view->errorMessage());
}

static void expectViewSettingYieldsErrorMessageOnPlay(
	std::function<void(ViewStub &)> transformation,
	std::string message)
{
	const auto view = std::make_shared<ViewStub>();
	Presenter presenter{ std::make_shared<ModelStub>(), view };
	transformation(*view);
	view->play();
	assertEqual(message, view->errorMessage());
}

TEST(PresenterErrorTests, nonFloatsThrowRequestFailures) {
	expectViewSettingYieldsErrorMessageOnPlay(
		[](ViewStub & view) {
			view.setLevel_dB_Spl("a");
		},
		"'a' is not a valid level.");
	expectViewSettingYieldsErrorMessageOnPlay(
		[](ViewStub & view) {
			view.setAttack_ms("b");
		},
		"'b' is not a valid attack time.");
	expectViewSettingYieldsErrorMessageOnPlay(
		[](ViewStub & view) {
			view.setRelease_ms("c");
		},
		"'c' is not a valid release time.");
}

static void expectBadWindowSize(std::string size) {
	expectViewSettingYieldsErrorMessageOnPlay(
		[=](ViewStub & view) {
			view.setWindowSize(size);
		},
		"'" + size + "' is not a valid window size.");
}

static void expectBadChunkSize(std::string size) {
	expectViewSettingYieldsErrorMessageOnPlay(
		[=](ViewStub & view) {
			view.setChunkSize(size);
		},
		"'" + size + "' is not a valid chunk size.");
}

TEST(PresenterErrorTests, nonPositiveIntegersThrowRequestFailures) {
	for (const auto s : std::vector<std::string>{ "a", "0.1", "-1" }) {
		expectBadWindowSize(s);
		expectBadChunkSize(s);
	}
}
