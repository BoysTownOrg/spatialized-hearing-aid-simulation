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

class INotSureYet {
public:
	struct CommonHearingAidSimulation {
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	INTERFACE_OPERATIONS(INotSureYet);
	virtual std::shared_ptr<AudioFrameProcessor> make(
		AudioFrameReader *reader,
		double level_dB_Spl
	) = 0;
};

class INotSureYetFactory {
public:
	INTERFACE_OPERATIONS(INotSureYetFactory);
	virtual std::shared_ptr<INotSureYet> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse) = 0;
	virtual std::shared_ptr<INotSureYet> makeHearingAid(
		INotSureYet::CommonHearingAidSimulation,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_) = 0;
	virtual std::shared_ptr<INotSureYet> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse,
		INotSureYet::CommonHearingAidSimulation,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_) = 0;
	virtual std::shared_ptr<INotSureYet> makeNoSimulation() = 0;
};

class nsySpatialization : public INotSureYet {
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

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader * reader, double level_dB_Spl) override
	{
		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;
		auto left_scale = gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel));
		auto right_scale = gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel));
		auto left_channel = simulationFactory->makeSpatialization(left_spatial, left_scale);
		auto right_channel = simulationFactory->makeSpatialization(right_spatial, right_scale);

		std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
		return std::make_shared<ChannelProcessingGroup>(channels);
	}
};

class nsyHearingAid : public INotSureYet {
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

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader * reader, double level_dB_Spl) override
	{
		left_hs.sampleRate = reader->sampleRate();
		right_hs.sampleRate = reader->sampleRate();

		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;
		auto left_scale = gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel));
		auto right_scale = gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel));
		auto left_channel = simulationFactory->makeHearingAidSimulation(left_hs, left_scale);
		auto right_channel = simulationFactory->makeHearingAidSimulation(right_hs, right_scale);

		std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
		return std::make_shared<ChannelProcessingGroup>(channels);
	}
};

class nsyFullSimulation : public INotSureYet {
	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	ISpatializedHearingAidSimulationFactory::HearingAidSimulation left_hs;
	ISpatializedHearingAidSimulationFactory::HearingAidSimulation right_hs;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	nsyFullSimulation(
		BrirReader::BinauralRoomImpulseResponse brir_,
		INotSureYet::CommonHearingAidSimulation processing,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } 
	{
		left_spatial.filterCoefficients = std::move(brir_.left);
		right_spatial.filterCoefficients = std::move(brir_.right);

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

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader * reader, double level_dB_Spl) override
	{
		left_hs.sampleRate = reader->sampleRate();
		right_hs.sampleRate = reader->sampleRate();

		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;
		auto left_scale = gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel));
		auto right_scale = gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel));
		
		ISpatializedHearingAidSimulationFactory::FullSimulation left_fs;
		left_fs.hearingAid = left_hs;
		left_fs.spatialization = left_spatial;

		ISpatializedHearingAidSimulationFactory::FullSimulation right_fs;
		right_fs.hearingAid = right_hs;
		right_fs.spatialization = right_spatial;

		auto left_channel = simulationFactory->makeFullSimulation(left_fs, left_scale);
		auto right_channel = simulationFactory->makeFullSimulation(right_fs, right_scale);

		std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
		return std::make_shared<ChannelProcessingGroup>(channels);
	}
};

class nsyNoSimulation : public INotSureYet {
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	nsyNoSimulation(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } {}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader * reader, double level_dB_Spl) override
	{
		auto computer = calibrationFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - RefactoredModel::fullScaleLevel_dB_Spl;
		auto left_scale = gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel));
		auto right_scale = gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel));
		auto left_channel = simulationFactory->makeWithoutSimulation(left_scale);
		auto right_channel = simulationFactory->makeWithoutSimulation(right_scale);
		std::vector<ChannelProcessingGroup::channel_processing_type> channels{ left_channel, right_channel };
		return std::make_shared<ChannelProcessingGroup>(channels);
	}
};

class NotSureYetFactory : public INotSureYetFactory {
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	NotSureYetFactory(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } {}

	std::shared_ptr<INotSureYet> makeSpatialization(BrirReader::BinauralRoomImpulseResponse brir) override
	{
		return std::make_shared<nsySpatialization>(brir, simulationFactory, calibrationFactory);
	}
	
	std::shared_ptr<INotSureYet> makeHearingAid(
		INotSureYet::CommonHearingAidSimulation common, 
		PrescriptionReader::Dsl leftPrescription_, 
		PrescriptionReader::Dsl rightPrescription_
	) override
	{
		return std::make_shared<nsyHearingAid>(common, leftPrescription_, rightPrescription_, simulationFactory, calibrationFactory);
	}

	std::shared_ptr<INotSureYet> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse brir, 
		INotSureYet::CommonHearingAidSimulation common, 
		PrescriptionReader::Dsl leftPrescription_, 
		PrescriptionReader::Dsl rightPrescription_
	) override
	{
		return std::make_shared<nsyFullSimulation>(brir, common, leftPrescription_, rightPrescription_, simulationFactory, calibrationFactory);
	}

	std::shared_ptr<INotSureYet> makeNoSimulation() override
	{
		return std::make_shared<nsyNoSimulation>(simulationFactory, calibrationFactory);
	}
};

class NotSureYet {
	std::shared_ptr<INotSureYet> nsy{};
public:
	NotSureYet(
		BrirReader::BinauralRoomImpulseResponse brir_,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		RefactoredModel::ProcessingParameters processing,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	)
	{
		NotSureYetFactory factory{simulationFactory, calibrationFactory};

		INotSureYet::CommonHearingAidSimulation common;
		common.attack_ms = processing.attack_ms;
		common.release_ms = processing.release_ms;
		common.chunkSize = processing.chunkSize;
		common.windowSize = processing.windowSize;

		if (processing.usingHearingAidSimulation && processing.usingSpatialization)
			nsy = factory.makeFullSimulation(brir_, common, leftPrescription_, rightPrescription_);
		else if (processing.usingSpatialization)
			nsy = factory.makeSpatialization(brir_);
		else if (processing.usingHearingAidSimulation)
			nsy = factory.makeHearingAid(common, leftPrescription_, rightPrescription_);
		else
			nsy = factory.makeNoSimulation();
	}

	std::shared_ptr<AudioFrameProcessor> make(
		AudioFrameReader *reader,
		double level_dB_Spl
	) {
		return nsy->make(reader, level_dB_Spl);
	}
};

void RefactoredModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return;

	NotSureYet notSure{
		brir,
		leftPrescription,
		rightPrescription,
		testParameters.processing,
		simulationFactory,
		calibrationFactory
	};
	auto reader = makeReader(perceptionTest->nextStimulus());
	loader->setProcessor(notSure.make(reader.get(), p.level_dB_Spl));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, testParameters.processing, std::move(p.audioDevice));
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

	NotSureYet notSure{
		std::move(brir_),
		std::move(leftPrescription_),
		std::move(rightPrescription_),
		p.processing,
		simulationFactory,
		calibrationFactory
	};
	auto reader = makeReader(p.audioFilePath);
	loader->setProcessor(notSure.make(reader.get(), p.level_dB_Spl));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, p.processing, p.audioDevice);
	player->play();
}

void RefactoredModel::stopCalibration() {
}

std::vector<std::string> RefactoredModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
