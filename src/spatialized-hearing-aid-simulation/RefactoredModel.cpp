#include "RefactoredModel.h"
#include "ChannelProcessingGroup.h"
#include <gsl/gsl>

class nsySpatialization : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	nsySpatialization(
		BrirReader::BinauralRoomImpulseResponse brir_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } 
	{
		left_spatial.filterCoefficients = std::move(brir_.left);
		right_spatial.filterCoefficients = std::move(brir_.right);
	}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *reader, double level_dB_Spl) override {
		return std::make_shared<ChannelProcessingGroup>(makeChannels(reader, level_dB_Spl));
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> makeChannels(
		AudioFrameReader *reader, 
		double level_dB_Spl
	) {
		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;

		return { 
			simulationFactory->makeSpatialization(
				left_spatial, 
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeSpatialization(
				right_spatial, 
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			) 
		};
	}
};

class nsyHearingAid : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory::HearingAidSimulation left_hs;
	ISpatializedHearingAidSimulationFactory::HearingAidSimulation right_hs;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	nsyHearingAid(
		CommonHearingAidSimulation processing,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } 
	{
		ISpatializedHearingAidSimulationFactory::HearingAidSimulation both_hs;
		both_hs.attack_ms = processing.attack_ms;
		both_hs.release_ms = processing.release_ms;
		both_hs.chunkSize = processing.chunkSize;
		both_hs.windowSize = processing.windowSize;
		both_hs.fullScaleLevel_dB_Spl = RefactoredModel::fullScaleLevel_dB_Spl;

		left_hs = both_hs;
		right_hs = both_hs;
		left_hs.prescription = std::move(leftPrescription_);
		right_hs.prescription = std::move(rightPrescription_);
	}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *reader, double level_dB_Spl) override {
		return std::make_shared<ChannelProcessingGroup>(makeChannels(reader, level_dB_Spl));
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> makeChannels(
		AudioFrameReader *reader, 
		double level_dB_Spl
	) {
		left_hs.sampleRate = reader->sampleRate();
		right_hs.sampleRate = reader->sampleRate();

		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;

		return { 
			simulationFactory->makeHearingAidSimulation(
				left_hs, 
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeHearingAidSimulation(
				right_hs, 
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			) 
		};
	}
};

class nsyFullSimulation : public AudioFrameProcessorFactory {	
	ISpatializedHearingAidSimulationFactory::FullSimulation left_fs;	
	ISpatializedHearingAidSimulationFactory::FullSimulation right_fs;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	nsyFullSimulation(
		BrirReader::BinauralRoomImpulseResponse brir_,
		AudioFrameProcessorFactory::CommonHearingAidSimulation processing,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } 
	{
		left_fs.spatialization.filterCoefficients = std::move(brir_.left);
		right_fs.spatialization.filterCoefficients = std::move(brir_.right);

		ISpatializedHearingAidSimulationFactory::HearingAidSimulation both_hs;
		both_hs.attack_ms = processing.attack_ms;
		both_hs.release_ms = processing.release_ms;
		both_hs.chunkSize = processing.chunkSize;
		both_hs.windowSize = processing.windowSize;
		both_hs.fullScaleLevel_dB_Spl = RefactoredModel::fullScaleLevel_dB_Spl;

		left_fs.hearingAid = both_hs;
		right_fs.hearingAid = both_hs;
		left_fs.hearingAid.prescription = std::move(leftPrescription_);
		right_fs.hearingAid.prescription = std::move(rightPrescription_);
	}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *reader, double level_dB_Spl) override {
		return std::make_shared<ChannelProcessingGroup>(makeChannels(reader, level_dB_Spl));
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> makeChannels(
		AudioFrameReader *reader, 
		double level_dB_Spl
	) {
		left_fs.hearingAid.sampleRate = reader->sampleRate();
		right_fs.hearingAid.sampleRate = reader->sampleRate();

		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;

		return { 
			simulationFactory->makeFullSimulation(
				left_fs, 
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeFullSimulation(
				right_fs, 
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			) 
		};
	}
};

class nsyNoSimulation : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	nsyNoSimulation(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) noexcept :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } {}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *reader, double level_dB_Spl) override {
		return std::make_shared<ChannelProcessingGroup>(makeChannels(reader, level_dB_Spl));
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> makeChannels(
		AudioFrameReader *reader, 
		double level_dB_Spl
	) {
		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;
		return { 
			simulationFactory->makeWithoutSimulation(
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeWithoutSimulation(
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			)
		};
	}
};

class NotSureYetFactory : public AudioFrameProcessorFactoryFactory {
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	NotSureYetFactory(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) noexcept :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } {}

	std::shared_ptr<AudioFrameProcessorFactory> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse brir
	) override {
		return std::make_shared<nsySpatialization>(
			std::move(brir), 
			simulationFactory, 
			calibrationFactory
		);
	}
	
	std::shared_ptr<AudioFrameProcessorFactory> makeHearingAid(
		AudioFrameProcessorFactory::CommonHearingAidSimulation common, 
		PrescriptionReader::Dsl leftPrescription_, 
		PrescriptionReader::Dsl rightPrescription_
	) override
	{
		return std::make_shared<nsyHearingAid>(
			std::move(common), 
			std::move(leftPrescription_), 
			std::move(rightPrescription_), 
			simulationFactory, 
			calibrationFactory
		);
	}

	std::shared_ptr<AudioFrameProcessorFactory> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse brir, 
		AudioFrameProcessorFactory::CommonHearingAidSimulation common, 
		PrescriptionReader::Dsl leftPrescription_, 
		PrescriptionReader::Dsl rightPrescription_
	) override
	{
		return std::make_shared<nsyFullSimulation>(
			std::move(brir), 
			std::move(common), 
			std::move(leftPrescription_), 
			std::move(rightPrescription_), 
			simulationFactory, 
			calibrationFactory
		);
	}

	std::shared_ptr<AudioFrameProcessorFactory> makeNoSimulation() override {
		return std::make_shared<nsyNoSimulation>(
			simulationFactory, 
			calibrationFactory
		);
	}
};

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
	ICalibrationComputerFactory *calibrationFactory
) :
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	perceptionTest{ perceptionTest },
	audioReaderFactory{ audioReaderFactory },
	player{ player },
	loader{ loader },
	processorFactoryFactory{std::make_shared<NotSureYetFactory>(simulationFactory, calibrationFactory)}
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
	brirForTest = readBrir(std::move(p.processing.brirFilePath));
	if (brirForTest.left.empty())
		throw RequestFailure{ coefficientErrorMessage("left") };
	if (brirForTest.right.empty())
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
	readPrescriptionsForTest(p);
	checkSizeIsPowerOfTwo(p.processing.chunkSize);
	checkSizeIsPowerOfTwo(p.processing.windowSize);
}

void RefactoredModel::readPrescriptionsForTest(TestParameters p) {
	leftPrescriptionForTest = readPrescription(std::move(p.processing.leftDslPrescriptionFilePath));
	rightPrescriptionForTest = readPrescription(std::move(p.processing.rightDslPrescriptionFilePath));
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
	if (player->isPlaying())
		return;

	auto processorFactory = makeAudioFrameProcessorFactory(
		brirForTest,
		leftPrescriptionForTest,
		rightPrescriptionForTest,
		testParameters.processing
	);
	auto reader = makeReader(perceptionTest->nextStimulus());
	loader->setProcessor(processorFactory->make(reader.get(), p.level_dB_Spl));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, testParameters.processing, std::move(p.audioDevice));
	player->play();
	perceptionTest->advanceTrial();
}

std::shared_ptr<AudioFrameProcessorFactory> RefactoredModel::makeAudioFrameProcessorFactory(
	BrirReader::BinauralRoomImpulseResponse brir_,
	PrescriptionReader::Dsl leftPrescription_,
	PrescriptionReader::Dsl rightPrescription_,
	ProcessingParameters processing
) {
	AudioFrameProcessorFactory::CommonHearingAidSimulation common;
	common.attack_ms = processing.attack_ms;
	common.release_ms = processing.release_ms;
	common.chunkSize = processing.chunkSize;
	common.windowSize = processing.windowSize;

	if (processing.usingHearingAidSimulation && processing.usingSpatialization)
		return processorFactoryFactory->makeFullSimulation(
			std::move(brir_), 
			std::move(common), 
			std::move(leftPrescription_), 
			std::move(rightPrescription_)
		);
	else if (processing.usingSpatialization)
		return processorFactoryFactory->makeSpatialization(std::move(brir_));
	else if (processing.usingHearingAidSimulation)
		return processorFactoryFactory->makeHearingAid(
			std::move(common), 
			std::move(leftPrescription_), 
			std::move(rightPrescription_)
		);
	else
		return processorFactoryFactory->makeNoSimulation();
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
	if (player->isPlaying())
		return;

	BrirReader::BinauralRoomImpulseResponse brir_;
	if (p.processing.usingSpatialization)
		brir_ = readBrir(p.processing.brirFilePath);
	PrescriptionReader::Dsl leftPrescription_;
	PrescriptionReader::Dsl rightPrescription_;
	if (p.processing.usingHearingAidSimulation) {
		leftPrescription_ = readPrescription(p.processing.leftDslPrescriptionFilePath);
		rightPrescription_ = readPrescription(p.processing.rightDslPrescriptionFilePath);
	}

	auto processorFactory = makeAudioFrameProcessorFactory(
		std::move(brir_),
		std::move(leftPrescription_),
		std::move(rightPrescription_),
		p.processing
	);
	auto reader = makeReader(std::move(p.audioFilePath));
	loader->setProcessor(processorFactory->make(reader.get(), p.level_dB_Spl));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, std::move(p.processing), std::move(p.audioDevice));
	player->play();
}

void RefactoredModel::stopCalibration() {
}

std::vector<std::string> RefactoredModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
