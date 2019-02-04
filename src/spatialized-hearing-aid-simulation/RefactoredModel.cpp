#include "RefactoredModel.h"
#include "ChannelProcessingGroup.h"
#include <gsl/gsl>

// The MATLAB hearing aid simulation uses 119 dB SPL as a "max"
double const RefactoredModel::fullScaleLevel_dB_Spl = 119;
int const RefactoredModel::defaultFramesPerBuffer = 1024;

RefactoredModel::RefactoredModel(
	SpeechPerceptionTest *perceptionTest,
	IAudioPlayer *player,
	AudioLoader *loader,
	AudioFrameReaderFactory *audioReaderFactory,
	PrescriptionReader *prescriptionReader,
	BrirReader *brirReader,
	ISpatializedHearingAidSimulationFactory *simulationFactory
) :
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	perceptionTest{ perceptionTest },
	audioReaderFactory{ audioReaderFactory },
	player{ player },
	loader{ loader },
	simulationFactory{ simulationFactory }
{
	player->setAudioLoader(loader);
}

void RefactoredModel::prepareNewTest(TestParameters p) {
	checkAndStore(p);
	prepareNewTest_(std::move(p));
}

void RefactoredModel::checkAndStore(TestParameters p) {
	if (p.processing.usingSpatialization)
		checkAndStoreBrir(p);
	if (p.processing.usingHearingAidSimulation)
		checkAndStorePrescriptions(p);
	testParameters = std::move(p);
}

static std::string coefficientErrorMessage(std::string which) {
	return 
		"The " + which + " BRIR coefficients are empty, "
		"therefore a filter operation cannot be defined.";
}

void RefactoredModel::checkAndStoreBrir(TestParameters p) {
	brir = readBrir(std::move(p.processing.brirFilePath));
	if (brir.left.empty())
		throw RequestFailure{ coefficientErrorMessage("left") };
	if (brir.right.empty())
		throw RequestFailure{ coefficientErrorMessage("right") };
}

BrirReader::BinauralRoomImpulseResponse RefactoredModel::readBrir(std::string filePath) {
	try {
		return brirReader->read(filePath);
	}
	catch (const BrirReader::ReadFailure &) {
		throw RequestFailure{ "Unable to read '" + filePath + "'." };
	}
}

void RefactoredModel::checkAndStorePrescriptions(TestParameters p) {
	readPrescriptions(p);
	checkSizeIsPowerOfTwo(p.processing.chunkSize);
	checkSizeIsPowerOfTwo(p.processing.windowSize);
}

void RefactoredModel::readPrescriptions(TestParameters p) {
	leftPrescription = readPrescription(std::move(p.processing.leftDslPrescriptionFilePath));
	rightPrescription = readPrescription(std::move(p.processing.rightDslPrescriptionFilePath));
}

PrescriptionReader::Dsl RefactoredModel::readPrescription(std::string filePath) {
	try {
		return prescriptionReader->read(filePath);
	}
	catch (const PrescriptionReader::ReadFailure &) {
		throw RequestFailure{ "Unable to read '" + filePath + "'." };
	}
}

static constexpr bool powerOfTwo(int n) noexcept {
	return n > 0 && (n & (n - 1)) == 0;
}

static std::string windowChunkSizesErrorMessage(int offender) {
	return
		"Both the chunk size and window size must be powers of two; " +
		std::to_string(offender) + " is not a power of two.";
}

void RefactoredModel::checkSizeIsPowerOfTwo(int size) {
	if (!powerOfTwo(size))
		throw RequestFailure{ windowChunkSizesErrorMessage(size) };
}

void RefactoredModel::prepareNewTest_(TestParameters p) {
	SpeechPerceptionTest::TestParameters adapted;
	adapted.audioDirectory = std::move(p.audioDirectory);
	adapted.testFilePath = std::move(p.testFilePath);
	adapted.subjectId = std::move(p.subjectId);
	adapted.testerId = std::move(p.testerId);
	try {
		perceptionTest->prepareNewTest(std::move(adapted));
	}
	catch (const SpeechPerceptionTest::TestInitializationFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

class RmsComputer {
	using sample_type = AudioFrameReader::channel_type::element_type;
	using channel_type = std::vector<sample_type>;
    std::vector<channel_type> audioFileContents;
public:
	explicit RmsComputer(AudioFrameReader &reader) :
		audioFileContents(
			reader.channels(), 
			channel_type(gsl::narrow<channel_type::size_type>(reader.frames()))
		)
	{
		read(reader);
	}

	void read(AudioFrameReader & reader) {
		std::vector<AudioFrameReader::channel_type> adapted;
		for (auto &channel : audioFileContents)
			adapted.push_back({ channel });
		reader.read(adapted);
		reader.reset();
	}

    auto compute(int channel) {
		return rms(audioFileContents.at(channel));
    }

private:
	template<typename T>
	T rms(std::vector<T> x) {
		return std::sqrt(
			std::accumulate(
				x.begin(),
				x.end(),
				T{ 0 },
				[](T a, T b) { return a += b * b; }
			) / x.size()
		);
	}
};

void RefactoredModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return;

	auto reader = makeReader(perceptionTest->nextStimulus());
    RmsComputer rms{ *reader };
    const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - fullScaleLevel_dB_Spl) / 20.0);
	const auto left_scale = reader->channels() > 0
		? gsl::narrow_cast<float>(desiredRms / rms.compute(0))
		: 0;
	const auto right_scale = reader->channels() > 1
		? gsl::narrow_cast<float>(desiredRms / rms.compute(1))
		: 0;

	auto left_channel = simulationFactory->makeWithoutSimulation(left_scale);
	auto right_channel = simulationFactory->makeWithoutSimulation(right_scale);

	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	left_spatial.filterCoefficients = brir.left;

	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	right_spatial.filterCoefficients = brir.right;

	ISpatializedHearingAidSimulationFactory::HearingAidSimulation both_hs;
	both_hs.attack_ms = testParameters.processing.attack_ms;
	both_hs.release_ms = testParameters.processing.release_ms;
	both_hs.chunkSize = testParameters.processing.chunkSize;
	both_hs.windowSize = testParameters.processing.windowSize;
	both_hs.sampleRate = reader->sampleRate();
	both_hs.fullScaleLevel_dB_Spl = fullScaleLevel_dB_Spl;

	auto left_hs = both_hs;
	left_hs.prescription = leftPrescription;

	auto right_hs = both_hs;
	right_hs.prescription = rightPrescription;

	ISpatializedHearingAidSimulationFactory::FullSimulation left_fs;
	left_fs.hearingAid = left_hs;
	left_fs.spatialization = left_spatial;

	ISpatializedHearingAidSimulationFactory::FullSimulation right_fs;
	right_fs.hearingAid = right_hs;
	right_fs.spatialization = right_spatial;

	if (testParameters.processing.usingSpatialization) {
		left_channel = simulationFactory->makeSpatialization(left_spatial, left_scale);
		right_channel = simulationFactory->makeSpatialization(right_spatial, right_scale);
	}
	if (testParameters.processing.usingHearingAidSimulation) {
		left_channel = simulationFactory->makeHearingAidSimulation(left_hs, left_scale);
		right_channel = simulationFactory->makeHearingAidSimulation(right_hs, right_scale);
		if (testParameters.processing.usingSpatialization) {
			left_channel = simulationFactory->makeFullSimulation(left_fs, left_scale);
			right_channel = simulationFactory->makeFullSimulation(right_fs, right_scale);
		}
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
	loader->setProcessor(std::make_shared<ChannelProcessingGroup>(channels));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, testParameters.processing, p.audioDevice);
	player->play();
	perceptionTest->advanceTrial();
}

std::shared_ptr<AudioFrameReader> RefactoredModel::makeReader(std::string filePath) {
	try {
		return audioReaderFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

void RefactoredModel::prepareAudioPlayer(
	AudioFrameReader &reader, 
	ProcessingParameters processing, 
	std::string audioDevice
) {
	IAudioPlayer::Preparation playing{};
	playing.channels = reader.channels();
	playing.framesPerBuffer = processing.usingHearingAidSimulation
		? processing.chunkSize
		: defaultFramesPerBuffer;
	playing.sampleRate = reader.sampleRate();
	playing.audioDevice = std::move(audioDevice);
	try {
		player->prepareToPlay(std::move(playing));
	}
	catch (const IAudioPlayer::PreparationFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

bool RefactoredModel::testComplete() {
	return perceptionTest->testComplete();
}

void RefactoredModel::playCalibration(CalibrationParameters p) {
	readBrir(p.processing.brirFilePath);
	auto reader = makeReader(p.audioFilePath);
	loader->setReader(reader);
	player->play();
	prepareAudioPlayer(*reader, p.processing, p.audioDevice);
	reader->reset();
	readPrescription(p.processing.leftDslPrescriptionFilePath);
}

void RefactoredModel::stopCalibration() {
}

std::vector<std::string> RefactoredModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
