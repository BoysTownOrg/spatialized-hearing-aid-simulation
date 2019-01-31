#include "RefactoredModel.h"
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ChannelProcessingGroup.h>
#include <gsl/gsl>

// The MATLAB hearing aid simulation uses 119 dB SPL as a "max"
double const RefactoredModel::fullScaleLevel_dB_Spl = 119;

RefactoredModel::RefactoredModel(
	SpeechPerceptionTest *perceptionTest,
	IAudioPlayer *player,
	AudioLoader *loader,
	AudioFrameReaderFactory *audioReaderFactory,
	HearingAidFactory *hearingAidFactory,
	PrescriptionReader *prescriptionReader,
	FirFilterFactory *firFilterFactory,
	BrirReader *brirReader,
	ScalarFactory *scalarFactory,
	IRefactoredSpatializedHearingAidSimulationFactory *simulationFactory
) :
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	perceptionTest{ perceptionTest },
	hearingAidFactory{ hearingAidFactory },
	firFilterFactory{ firFilterFactory },
	scalarFactory{ scalarFactory },
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
	auto reader = audioReaderFactory->make(perceptionTest->nextStimulus());
	const auto leftChannel = std::make_shared<SignalProcessingChain>();
	const auto rightChannel = std::make_shared<SignalProcessingChain>();
    RmsComputer rms{ *reader };
    const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - fullScaleLevel_dB_Spl) / 20.0);
	if (reader->channels() > 0) {
		IRefactoredSpatializedHearingAidSimulationFactory::SimulationParameters sp;
		sp.scale = gsl::narrow_cast<float>(desiredRms / rms.compute(0));
		sp.prescription = leftPrescription;
		sp.attack_ms = testParameters.attack_ms;
		sp.release_ms = testParameters.release_ms;
		sp.chunkSize = testParameters.chunkSize;
		sp.windowSize = testParameters.windowSize;
		simulationFactory->make(sp);
		leftChannel->add(scalarFactory->make(gsl::narrow_cast<float>(desiredRms / rms.compute(0))));
	}
	if (reader->channels() > 1) {
		IRefactoredSpatializedHearingAidSimulationFactory::SimulationParameters sp;
		sp.scale = gsl::narrow_cast<float>(desiredRms / rms.compute(1));
		sp.prescription = rightPrescription;
		sp.attack_ms = testParameters.attack_ms;
		sp.release_ms = testParameters.release_ms;
		sp.chunkSize = testParameters.chunkSize;
		sp.windowSize = testParameters.windowSize;
		simulationFactory->make(sp);
		rightChannel->add(scalarFactory->make(gsl::narrow_cast<float>(desiredRms / rms.compute(1))));
	}
	if (testParameters.usingSpatialization) {
		leftChannel->add(firFilterFactory->make(brir.left));
		rightChannel->add(firFilterFactory->make(brir.right));
	}
	if (testParameters.usingHearingAidSimulation) {
		leftChannel->add(makeHearingAid(leftPrescription, reader->sampleRate()));
		rightChannel->add(makeHearingAid(rightPrescription, reader->sampleRate()));
	}
	std::vector<ChannelProcessingGroup::channel_processing_type> channels{ leftChannel, rightChannel };
	loader->setProcessor(std::make_shared<ChannelProcessingGroup>(channels));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, p);
	player->play();
	perceptionTest->advanceTrial();
}

void RefactoredModel::prepareAudioPlayer(AudioFrameReader & reader, TrialParameters p) {
	IAudioPlayer::Preparation playing{};
	playing.channels = reader.channels();
	playing.framesPerBuffer = testParameters.chunkSize;
	playing.sampleRate = reader.sampleRate();
	playing.audioDevice = p.audioDevice;
	try {
		player->prepareToPlay(playing);
	}
	catch (const IAudioPlayer::PreparationFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> RefactoredModel::makeHearingAid(
	PrescriptionReader::Dsl dsl, 
	int sampleRate
) {
	FilterbankCompressor::Parameters compression;
	compression.attack_ms = testParameters.attack_ms;
	compression.release_ms = testParameters.release_ms;
	compression.chunkSize = testParameters.chunkSize;
	compression.windowSize = testParameters.windowSize;
	compression.sampleRate = sampleRate;
	compression.compressionRatios = dsl.compressionRatios;
	compression.crossFrequenciesHz = dsl.crossFrequenciesHz;
	compression.kneepointGains_dB = dsl.kneepointGains_dB;
	compression.kneepoints_dBSpl = dsl.kneepoints_dBSpl;
	compression.broadbandOutputLimitingThresholds_dBSpl = 
		dsl.broadbandOutputLimitingThresholds_dBSpl;
	compression.channels = dsl.channels;
	compression.max_dB_Spl = fullScaleLevel_dB_Spl;
	return hearingAidFactory->make(compression);
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
