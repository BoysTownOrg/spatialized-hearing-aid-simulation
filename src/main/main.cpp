#include <audio-file-reading/AudioFileInMemory.h>
#include <sndfile.h>

class LibsndfileReader : public AudioFileReader {
	SNDFILE *file{};
	SF_INFO info{};
public:
	explicit LibsndfileReader(std::string filePath);
	~LibsndfileReader();
	void readFrames(float *, long long) override;
	long long frames() override;
	int channels() override;
};

LibsndfileReader::LibsndfileReader(std::string filePath) {
	file = sf_open(filePath.c_str(), SFM_READ, &info);
}

LibsndfileReader::~LibsndfileReader() {
	sf_close(file);
}

void LibsndfileReader::readFrames(float *x, long long count) {
	sf_readf_float(file, x, count);
}

long long LibsndfileReader::frames() {
	return info.frames;
}

int LibsndfileReader::channels() {
	return info.channels;
}

#include <presentation/Presenter.h>
#define WIN32
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl.H>

class FltkWindow : public View
{
	// FLTK forms widget groups at construction.
	// Therefore window is declared first.
	Fl_Double_Window window;
	Fl_Input _prescriptionFilePath;
	Fl_Input _audioFilePath;
	Fl_Input _brirFilePath;
	Fl_Button browsePrescription;
	Fl_Button browseAudio;
	Fl_Button browseBrir;
	Presenter *presenter{};
	int browseResult{};
public:
	FltkWindow();
	void setPresenter(Presenter *) override;
	void runEventLoop() override;
	std::string browseForFile(std::vector<std::string> filters) override;
	bool browseCancelled() override;
	void setDslPrescriptionFilePath(std::string) override;
	void setAudioFilePath(std::string) override;
	void setBrirFilePath(std::string) override;
	std::string dslPrescriptionFilePath() const override;
	std::string audioFilePath() const override;
	std::string brirFilePath() const override;

private:
	static void onBrowsePrescription(Fl_Widget *, void *);
	static void onBrowseAudio(Fl_Widget *, void *);
	static void onBrowseBrir(Fl_Widget *, void *);
};

#include <sstream>

void FltkWindow::onBrowsePrescription(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForDslPrescription();
}

void FltkWindow::onBrowseAudio(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForAudio();
}

void FltkWindow::onBrowseBrir(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForBrir();
}

FltkWindow::FltkWindow() :
	window(500, 300, 450, 300),
	_prescriptionFilePath(150, 50, 200, 40, "Prescription file path"),
	_audioFilePath(150, 100, 200, 40, "Audio file path"),
	_brirFilePath(150, 150, 200, 40, "BRIR file path"),
	browsePrescription(360, 50, 60, 40, "Browse"),
	browseAudio(360, 100, 60, 40, "Browse"),
	browseBrir(360, 150, 60, 40, "Browse")
{
	window.end();
	window.show();
	browsePrescription.callback(onBrowsePrescription, this);
	browseAudio.callback(onBrowseAudio, this);
	browseBrir.callback(onBrowseBrir, this);
}

void FltkWindow::runEventLoop() {
	Fl::run();
}

void FltkWindow::setPresenter(Presenter *c) {
	presenter = c;
}

static std::string formatFilters(std::vector<std::string> filters) {
	std::stringstream stream;
	for (const auto &filter : filters)
		stream << filter << "\n";
	return stream.str();
}

std::string FltkWindow::browseForFile(std::vector<std::string> filters) {
	Fl_Native_File_Chooser chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
	chooser.filter(formatFilters(filters).c_str());
	browseResult = chooser.show();
	return chooser.filename();
}

enum class BrowseResult {
	cancelled = 1
};

bool FltkWindow::browseCancelled() {
	return browseResult == static_cast<int>(BrowseResult::cancelled);
}

void FltkWindow::setDslPrescriptionFilePath(std::string p) {
	_prescriptionFilePath.value(p.c_str());
}

void FltkWindow::setAudioFilePath(std::string p) {
	_audioFilePath.value(p.c_str());
}

void FltkWindow::setBrirFilePath(std::string p) {
	_brirFilePath.value(p.c_str());
}

std::string FltkWindow::dslPrescriptionFilePath() const {
	return _prescriptionFilePath.value();
}

std::string FltkWindow::audioFilePath() const {
	return _audioFilePath.value();
}

std::string FltkWindow::brirFilePath() const {
	return _brirFilePath.value();
}

int main() {}