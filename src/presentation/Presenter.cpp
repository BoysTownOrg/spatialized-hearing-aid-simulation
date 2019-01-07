#include "Presenter.h"

Presenter::Presenter(
	std::shared_ptr<Model> model, 
	std::shared_ptr<View> view
) :
	model{ std::move(model) },
	view{ std::move(view) }
{
	this->view->subscribe(this);
	this->view->populateAudioDeviceMenu(this->model->audioDeviceDescriptions());
}

void Presenter::run() {
	view->runEventLoop();
}

void Presenter::browseForLeftDslPrescription() {
	browseAndUpdateIfNotCancelled(
		{ "*.json" },
		[=](std::string p) { this->view->setLeftDslPrescriptionFilePath(p); });
}

void Presenter::browseForRightDslPrescription() {
	browseAndUpdateIfNotCancelled(
		{ "*.json" },
		[=](std::string p) { this->view->setRightDslPrescriptionFilePath(p); });
}

void Presenter::browseForAudio() {
	const auto directory = view->browseForDirectory();
	if (!view->browseCancelled())
		view->setAudioDirectory(directory);
}

void Presenter::browseForBrir() {
	browseAndUpdateIfNotCancelled(
		{ "*.wav" },
		[=](std::string p) { this->view->setBrirFilePath(p); });
}

void Presenter::browseAndUpdateIfNotCancelled(
	std::vector<std::string> filters,
	std::function<void(std::string)> update)
{
	const auto filePath = view->browseForFile(filters);
	if (!view->browseCancelled())
		update(filePath);
}

void Presenter::play() {
	try {
		Model::PlayRequest request;
		request.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
		request.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
		request.brirFilePath = view->brirFilePath();
		request.audioDirectory = view->audioDirectory();
		request.audioDevice = view->audioDevice();
		request.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
		request.attack_ms = convertToDouble(view->attack_ms(), "attack time");
		request.release_ms = convertToDouble(view->release_ms(), "release time");
		request.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
		request.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
		model->play(request);
	}
	catch (const BadInput &e) {
		view->showErrorDialog(e.what());
	}
	catch (const Model::RequestFailure &failure) {
		view->showErrorDialog(failure.what());
	}
}

static std::string badInputMessage(std::string x, std::string identifier) {
	return { "'" + x + "' is not a valid " + identifier + "." };
}

double Presenter::convertToDouble(
	std::string x, 
	std::string identifier)
{
	try {
		return std::stod(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput{ badInputMessage(x, identifier) };
	}
}

static bool onlyContainsDigits(const std::string s) {
	return s.find_first_not_of("0123456789") == std::string::npos;
}

int Presenter::convertToPositiveInteger(std::string x, std::string identifier)
{
	if (!onlyContainsDigits(x))
		throw BadInput{ badInputMessage(x, identifier) };
	try {
		return std::stoi(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput{ badInputMessage(x, identifier) };
	}
}

void Presenter::newTest() {
	view->showTestSetup();
}

void Presenter::confirmTestSetup() {
	view->hideTestSetup();
	Model::TestParameters p;
	p.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	p.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	p.brirFilePath = view->brirFilePath();
	p.audioDirectory = view->audioDirectory();
	p.audioDevice = view->audioDevice();
	p.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
	p.attack_ms = convertToDouble(view->attack_ms(), "attack time");
	p.release_ms = convertToDouble(view->release_ms(), "release time");
	p.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
	p.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
	model->initializeTest(p);
}
