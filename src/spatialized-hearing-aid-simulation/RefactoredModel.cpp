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
	if (p.usingSpatialization)
		checkAndStoreBrir(p);
	if (p.usingHearingAidSimulation)
		checkAndStorePrescriptions(p);
	testParameters = std::move(p);
}

static std::string coefficientErrorMessage(std::string which) {
	return 
		"The " + which + " BRIR coefficients are empty, "
		"therefore a filter operation cannot be defined.";
}

void RefactoredModel::checkAndStoreBrir(TestParameters p) {
	brir = readBrir(std::move(p));
	if (brir.left.empty())
		throw TestInitializationFailure{ coefficientErrorMessage("left") };
	if (brir.right.empty())
		throw TestInitializationFailure{ coefficientErrorMessage("right") };
}

BrirReader::BinauralRoomImpulseResponse RefactoredModel::readBrir(TestParameters p) {
	try {
		return brirReader->read(p.brirFilePath);
	}
	catch (const BrirReader::ReadFailure &) {
		throw TestInitializationFailure{ "Unable to read '" + p.brirFilePath + "'." };
	}
}

void RefactoredModel::checkAndStorePrescriptions(TestParameters p) {
	readPrescriptions(p);
	checkSizeIsPowerOfTwo(p.chunkSize);
	checkSizeIsPowerOfTwo(p.windowSize);
}

void RefactoredModel::readPrescriptions(TestParameters p) {
	leftPrescription = readPrescription(p.leftDslPrescriptionFilePath);
	rightPrescription = readPrescription(p.rightDslPrescriptionFilePath);
}

PrescriptionReader::Dsl RefactoredModel::readPrescription(std::string filePath) {
	try {
		return prescriptionReader->read(filePath);
	}
	catch (const PrescriptionReader::ReadFailure &) {
		throw TestInitializationFailure{ "Unable to read '" + filePath + "'." };
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
		throw TestInitializationFailure{ windowChunkSizesErrorMessage(size) };
}

void RefactoredModel::prepareNewTest_(TestParameters p) {
	SpeechPerceptionTest::TestParameters adapted;
	adapted.audioDirectory = p.audioDirectory;
	adapted.testFilePath = p.testFilePath;
	adapted.subjectId = p.subjectId;
	adapted.testerId = p.testerId;
	try {
		perceptionTest->prepareNewTest(adapted);
	}
	catch (const SpeechPerceptionTest::TestInitializationFailure &e) {
		throw TestInitializationFailure{ e.what() };
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
	float left_scale = reader->channels() > 0
		? gsl::narrow_cast<float>(desiredRms / rms.compute(0))
		: 0;
	float right_scale = reader->channels() > 1
		? gsl::narrow_cast<float>(desiredRms / rms.compute(1))
		: 0;
	simulationFactory->makeWithoutSimulation(left_scale);
	simulationFactory->makeWithoutSimulation(right_scale);

	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	left_spatial.filterCoefficients = brir.left;

	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	right_spatial.filterCoefficients = brir.right;

	ISpatializedHearingAidSimulationFactory::HearingAidSimulation left_hs;
	left_hs.attack_ms = testParameters.attack_ms;
	left_hs.release_ms = testParameters.release_ms;
	left_hs.chunkSize = testParameters.chunkSize;
	left_hs.windowSize = testParameters.windowSize;
	left_hs.prescription = leftPrescription;
	left_hs.sampleRate = reader->sampleRate();
	left_hs.fullScaleLevel_dB_Spl = fullScaleLevel_dB_Spl;

	ISpatializedHearingAidSimulationFactory::HearingAidSimulation right_hs;
	right_hs.attack_ms = testParameters.attack_ms;
	right_hs.release_ms = testParameters.release_ms;
	right_hs.chunkSize = testParameters.chunkSize;
	right_hs.windowSize = testParameters.windowSize;
	right_hs.prescription = rightPrescription;
	right_hs.sampleRate = reader->sampleRate();
	right_hs.fullScaleLevel_dB_Spl = fullScaleLevel_dB_Spl;

	ISpatializedHearingAidSimulationFactory::FullSimulation left_fs;
	left_fs.hearingAid = left_hs;
	left_fs.spatialization = left_spatial;

	ISpatializedHearingAidSimulationFactory::FullSimulation right_fs;
	right_fs.hearingAid = right_hs;
	right_fs.spatialization = right_spatial;

	ChannelProcessingGroup::channel_processing_type left_channel;
	ChannelProcessingGroup::channel_processing_type right_channel;
	if (testParameters.usingSpatialization) {
		simulationFactory->makeSpatialization(left_spatial, left_scale);
		simulationFactory->makeSpatialization(right_spatial, right_scale);
	}
	if (testParameters.usingHearingAidSimulation) {
		left_channel = simulationFactory->makeHearingAidSimulation(left_hs, left_scale);
		right_channel = simulationFactory->makeHearingAidSimulation(right_hs, right_scale);
		if (testParameters.usingSpatialization) {
			left_channel = simulationFactory->makeFullSimulation(left_fs, left_scale);
			right_channel = simulationFactory->makeFullSimulation(right_fs, right_scale);
		}
	}
	ISpatializedHearingAidSimulationFactory::SimulationParameters sp;
	sp.attack_ms = testParameters.attack_ms;
	sp.release_ms = testParameters.release_ms;
	sp.chunkSize = testParameters.chunkSize;
	sp.windowSize = testParameters.windowSize;
	sp.fullScaleLevel_dB_Spl = fullScaleLevel_dB_Spl;
	sp.usingHearingAidSimulation = testParameters.usingHearingAidSimulation;
	sp.usingSpatialization = testParameters.usingSpatialization;
	sp.sampleRate = reader->sampleRate();

	if (reader->channels() > 0)
		sp.scale = gsl::narrow_cast<float>(desiredRms / rms.compute(0));
	sp.prescription = leftPrescription;
	sp.filterCoefficients = brir.left;
	simulationFactory->make(sp);
	
	if (reader->channels() > 1)
		sp.scale = gsl::narrow_cast<float>(desiredRms / rms.compute(1));
	sp.prescription = rightPrescription;
	sp.filterCoefficients = brir.right;
	simulationFactory->make(sp);

	std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
	loader->setProcessor(std::make_shared<ChannelProcessingGroup>(channels));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, p.audioDevice);
	player->play();
	perceptionTest->advanceTrial();
}

std::shared_ptr<AudioFrameReader> RefactoredModel::makeReader(std::string filePath) {
	try {
		return audioReaderFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw TrialFailure{ e.what() };
	}
}

void RefactoredModel::prepareAudioPlayer(AudioFrameReader & reader, std::string audioDevice) {
	IAudioPlayer::Preparation playing{};
	playing.channels = reader.channels();
	playing.framesPerBuffer = testParameters.usingHearingAidSimulation
		? testParameters.chunkSize
		: defaultFramesPerBuffer;
	playing.sampleRate = reader.sampleRate();
	playing.audioDevice = std::move(audioDevice);
	try {
		player->prepareToPlay(playing);
	}
	catch (const IAudioPlayer::PreparationFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

bool RefactoredModel::testComplete() {
	return perceptionTest->testComplete();
}

void RefactoredModel::playCalibration(CalibrationParameters) {
	try {
		player->prepareToPlay({});
	}
	catch (const IAudioPlayer::PreparationFailure &e) {
		throw CalibrationFailure{ e.what() };
	}
}

void RefactoredModel::stopCalibration() {
}

std::vector<std::string> RefactoredModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
