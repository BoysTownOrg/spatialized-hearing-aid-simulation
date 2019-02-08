#include "RefactoredModel.h"
#include "ChannelProcessingGroup.h"
#include <gsl/gsl>

class NullProcessorFactory : public AudioFrameProcessorFactory {
	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *, double) override
	{
		return {};
	}
};

class StereoSpatializationFactory : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	StereoSpatializationFactory(
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

class StereoHearingAidFactory : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory::HearingAidSimulation left_hs;
	ISpatializedHearingAidSimulationFactory::HearingAidSimulation right_hs;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	StereoHearingAidFactory(
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

class StereoSpatializedHearingAidSimulationFactory : public AudioFrameProcessorFactory {	
	ISpatializedHearingAidSimulationFactory::FullSimulation left_fs;	
	ISpatializedHearingAidSimulationFactory::FullSimulation right_fs;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	StereoSpatializedHearingAidSimulationFactory(
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

class StereoNoSimulation : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	StereoNoSimulation(
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

class StereoProcessorFactoryFactory : public AudioFrameProcessorFactoryFactory {
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	StereoProcessorFactoryFactory(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	) noexcept :
		simulationFactory{ simulationFactory },
		calibrationFactory{ calibrationFactory } {}

	std::shared_ptr<AudioFrameProcessorFactory> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse brir
	) override {
		return std::make_shared<StereoSpatializationFactory>(
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
		return std::make_shared<StereoHearingAidFactory>(
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
		return std::make_shared<StereoSpatializedHearingAidSimulationFactory>(
			std::move(brir), 
			std::move(common), 
			std::move(leftPrescription_), 
			std::move(rightPrescription_), 
			simulationFactory, 
			calibrationFactory
		);
	}

	std::shared_ptr<AudioFrameProcessorFactory> makeNoSimulation() override {
		return std::make_shared<StereoNoSimulation>(
			simulationFactory, 
			calibrationFactory
		);
	}
};

// The MATLAB hearing aid simulation uses 119 dB SPL as a "max"
double const RefactoredModel::fullScaleLevel_dB_Spl = 119;
int const RefactoredModel::defaultFramesPerBuffer = 1024;

RefactoredModel::RefactoredModel(
	StimulusList *list,
	Documenter *documenter,
	IAudioPlayer *player,
	AudioLoader *loader,
	AudioFrameReaderFactory *audioReaderFactory,
	PrescriptionReader *prescriptionReader,
	BrirReader *brirReader,
	ISpatializedHearingAidSimulationFactory *simulationFactory,
	ICalibrationComputerFactory *calibrationFactory
) :
	list{ list },
	documenter{ documenter },
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	audioReaderFactory{ audioReaderFactory },
	player{ player },
	loader{ loader },
	processorFactoryFactory{
		std::make_shared<StereoProcessorFactoryFactory>(
			simulationFactory, 
			calibrationFactory
		)
	},
	processorFactoryForTest{
		std::make_shared<NullProcessorFactory>()
	}
{
	player->setAudioLoader(loader);
}

void RefactoredModel::prepareNewTest(TestParameters *p) {
	framesPerBufferForTest = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	processorFactoryForTest = makeProcessorFactory(p->processing);
	prepareNewTest_(p);
}

std::shared_ptr<AudioFrameProcessorFactory> RefactoredModel::makeProcessorFactory(
	ProcessingParameters p
) {
	if (p.usingHearingAidSimulation) {
		assertSizeIsPowerOfTwo(p.chunkSize);
		assertSizeIsPowerOfTwo(p.windowSize);
	}

	AudioFrameProcessorFactory::CommonHearingAidSimulation common;
	common.attack_ms = p.attack_ms;
	common.release_ms = p.release_ms;
	common.chunkSize = p.chunkSize;
	common.windowSize = p.windowSize;

	std::shared_ptr<AudioFrameProcessorFactory> processorFactory_{};

	if (p.usingHearingAidSimulation && p.usingSpatialization)
		return processorFactoryFactory->makeFullSimulation(
			readAndCheckBrir(std::move(p.brirFilePath)), 
			common, 
			readPrescription(std::move(p.leftDslPrescriptionFilePath)), 
			readPrescription(std::move(p.rightDslPrescriptionFilePath))
		);
	else if (p.usingSpatialization)
		return processorFactoryFactory->makeSpatialization(
			readAndCheckBrir(std::move(p.brirFilePath))
		);
	else if (p.usingHearingAidSimulation)
		return processorFactoryFactory->makeHearingAid(
			common, 
			readPrescription(std::move(p.leftDslPrescriptionFilePath)), 
			readPrescription(std::move(p.rightDslPrescriptionFilePath))
		);
	else
		return processorFactoryFactory->makeNoSimulation();
}

static std::string coefficientErrorMessage(std::string which) {
	return 
		"The " + which + " BRIR coefficients are empty, "
		"therefore a filter operation cannot be defined.";
}

BrirReader::BinauralRoomImpulseResponse RefactoredModel::readAndCheckBrir(std::string filePath) {
	auto brir = readBrir(std::move(filePath));
	if (brir.left.empty())
		throw RequestFailure{ coefficientErrorMessage("left") };
	if (brir.right.empty())
		throw RequestFailure{ coefficientErrorMessage("right") };
	return brir;
}

BrirReader::BinauralRoomImpulseResponse RefactoredModel::readBrir(std::string filePath) {
	try {
		return brirReader->read(filePath);
	}
	catch (const BrirReader::ReadFailure &) {
		throw RequestFailure{ "BRIR '" + filePath + "' cannot be read." };
	}
}

PrescriptionReader::Dsl RefactoredModel::readPrescription(std::string filePath) {
	try {
		return prescriptionReader->read(filePath);
	}
	catch (const PrescriptionReader::ReadFailure &) {
		throw RequestFailure{ "Prescription '" + filePath + "' cannot be read." };
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

void RefactoredModel::assertSizeIsPowerOfTwo(int size) {
	if (!powerOfTwo(size))
		throw RequestFailure{ windowChunkSizesErrorMessage(size) };
}

void RefactoredModel::prepareNewTest_(TestParameters *p) {
	try {
		list->initialize(p->audioDirectory);
		documenter->initialize(p->testFilePath);
		documenter->documentTestParameters(p);
		nextStimulus_ = list->next();
	}
	catch (const std::runtime_error &e) {
		throw RequestFailure{ e.what() };
	}
}

void RefactoredModel::playNextTrial(TrialParameters *p) {
	if (player->isPlaying())
		return;

	auto reader = makeReader(nextStimulus_);
	loader->setProcessor(processorFactoryForTest->make(reader.get(), p->level_dB_Spl));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, framesPerBufferForTest, p->audioDevice);
	player->play();
	Documenter::TrialParameters documenting;
	documenting.level_dB_Spl = p->level_dB_Spl;
	documenting.stimulus = nextStimulus_;
	documenter->documentTrialParameters(std::move(documenting));
	nextStimulus_ = list->next();
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
	int framesPerBuffer, 
	std::string audioDevice
) {
	IAudioPlayer::Preparation playing;
	playing.channels = reader.channels();
	playing.sampleRate = reader.sampleRate();
	playing.framesPerBuffer = framesPerBuffer;
	playing.audioDevice = std::move(audioDevice);
	try {
		player->prepareToPlay(std::move(playing));
	}
	catch (const IAudioPlayer::PreparationFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

bool RefactoredModel::testComplete() {
	return list->empty();
}

void RefactoredModel::playCalibration(CalibrationParameters *p) {
	if (player->isPlaying())
		return;

	const auto framesPerBuffer = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	
	auto processorFactory_ = makeProcessorFactory(p->processing);

	auto reader = makeReader(p->audioFilePath);
	loader->setProcessor(processorFactory_->make(reader.get(), p->level_dB_Spl));
	loader->setReader(reader);
	loader->reset();
	prepareAudioPlayer(*reader, framesPerBuffer, p->audioDevice);
	player->play();
}

void RefactoredModel::stopCalibration() {
	player->stop();
}

std::vector<std::string> RefactoredModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
