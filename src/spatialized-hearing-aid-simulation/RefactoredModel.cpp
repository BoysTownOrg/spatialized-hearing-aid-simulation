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
	ISpatializedHearingAidSimulationFactory *simulationFactory,
	ICalibrationComputerFactory *calibrationComputer
) :
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	perceptionTest{ perceptionTest },
	audioReaderFactory{ audioReaderFactory },
	player{ player },
	loader{ loader },
	simulationFactory{ simulationFactory },
	calibrationFactory{ calibrationComputer }
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

void RefactoredModel::playTrial(TrialParameters p) {
	play_(
		brir,
		leftPrescription,
		rightPrescription,
		perceptionTest->nextStimulus(),
		testParameters.processing,
		p.level_dB_Spl,
		p.audioDevice
	);
}

void RefactoredModel::play_(
	BrirReader::BinauralRoomImpulseResponse brir_,
	PrescriptionReader::Dsl leftPrescription_,
	PrescriptionReader::Dsl rightPrescription_,
	std::string audioFilePath,
	ProcessingParameters processing,
	double level_dB_Spl_,
	std::string audioDevice_
) {
	if (player->isPlaying())
		return;

	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	left_spatial.filterCoefficients = brir_.left;

	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	right_spatial.filterCoefficients = brir_.right;

	auto reader = makeReader(audioFilePath);

	ISpatializedHearingAidSimulationFactory::HearingAidSimulation both_hs;
	both_hs.attack_ms = processing.attack_ms;
	both_hs.release_ms = processing.release_ms;
	both_hs.chunkSize = processing.chunkSize;
	both_hs.windowSize = processing.windowSize;
	both_hs.sampleRate = reader->sampleRate();
	both_hs.fullScaleLevel_dB_Spl = fullScaleLevel_dB_Spl;

	auto left_hs = both_hs;
	auto right_hs = both_hs;
	left_hs.prescription = leftPrescription_;
	right_hs.prescription = rightPrescription_;

	ISpatializedHearingAidSimulationFactory::FullSimulation left_fs;
	left_fs.hearingAid = left_hs;
	left_fs.spatialization = left_spatial;

	ISpatializedHearingAidSimulationFactory::FullSimulation right_fs;
	right_fs.hearingAid = right_hs;
	right_fs.spatialization = right_spatial;

	auto computer = calibrationFactory->make(reader.get());
	const auto digitalLevel = level_dB_Spl_ - fullScaleLevel_dB_Spl;
	auto left_scale = gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel));
	auto right_scale = gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel));

	auto left_channel = simulationFactory->makeWithoutSimulation(left_scale);
	auto right_channel = simulationFactory->makeWithoutSimulation(right_scale);

	if (processing.usingSpatialization) {
		left_channel = simulationFactory->makeSpatialization(left_spatial, left_scale);
		right_channel = simulationFactory->makeSpatialization(right_spatial, right_scale);
	}
	if (processing.usingHearingAidSimulation) {
		left_channel = simulationFactory->makeHearingAidSimulation(left_hs, left_scale);
		right_channel = simulationFactory->makeHearingAidSimulation(right_hs, right_scale);
		if (processing.usingSpatialization) {
			left_channel = simulationFactory->makeFullSimulation(left_fs, left_scale);
			right_channel = simulationFactory->makeFullSimulation(right_fs, right_scale);
		}
	}
	std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
	loader->setProcessor(std::make_shared<ChannelProcessingGroup>(channels));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, processing, audioDevice_);
	player->play();
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
	BrirReader::BinauralRoomImpulseResponse brir_;
	if (p.processing.usingSpatialization)
		brir_ = readBrir(p.processing.brirFilePath);
	PrescriptionReader::Dsl leftPrescription_;
	PrescriptionReader::Dsl rightPrescription_;
	if (p.processing.usingHearingAidSimulation) {
		leftPrescription_ = readPrescription(p.processing.leftDslPrescriptionFilePath);
		rightPrescription_ = readPrescription(p.processing.rightDslPrescriptionFilePath);
	}
	play_(
		brir_,
		leftPrescription_,
		rightPrescription_,
		p.audioFilePath,
		p.processing,
		p.level_dB_Spl,
		p.audioDevice
	);
}

void RefactoredModel::stopCalibration() {
}

std::vector<std::string> RefactoredModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
