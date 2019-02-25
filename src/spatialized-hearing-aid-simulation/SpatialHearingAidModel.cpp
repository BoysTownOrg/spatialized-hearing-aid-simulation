#include "SpatialHearingAidModel.h"
#include "ChannelProcessingGroup.h"
#include <gsl/gsl>

class NullProcessorFactory : public AudioFrameProcessorFactory {
	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *, double) override { return {}; }
};

class StereoSpatializationFactory : public AudioFrameProcessorFactory {
	ISpatializedHearingAidSimulationFactory::Spatialization left_spatial;
	ISpatializedHearingAidSimulationFactory::Spatialization right_spatial;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoSpatializationFactory(
		BrirReader::BinauralRoomImpulseResponse brir_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationComputerFactory{ calibrationComputerFactory } 
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
		auto computer = calibrationComputerFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - SpatialHearingAidModel::fullScaleLevel_dB_Spl;

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
	ICalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoHearingAidFactory(
		CommonHearingAidSimulation processing,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationComputerFactory{ calibrationComputerFactory } 
	{
		ISpatializedHearingAidSimulationFactory::HearingAidSimulation both_hs;
		both_hs.attack_ms = processing.attack_ms;
		both_hs.release_ms = processing.release_ms;
		both_hs.chunkSize = processing.chunkSize;
		both_hs.windowSize = processing.windowSize;
		both_hs.fullScaleLevel_dB_Spl = SpatialHearingAidModel::fullScaleLevel_dB_Spl;

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

		auto computer = calibrationComputerFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - SpatialHearingAidModel::fullScaleLevel_dB_Spl;

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
	ICalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoSpatializedHearingAidSimulationFactory(
		BrirReader::BinauralRoomImpulseResponse brir_,
		AudioFrameProcessorFactory::CommonHearingAidSimulation processing,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory
	) :
		simulationFactory{ simulationFactory },
		calibrationComputerFactory{ calibrationComputerFactory } 
	{
		left_fs.spatialization.filterCoefficients = std::move(brir_.left);
		right_fs.spatialization.filterCoefficients = std::move(brir_.right);

		ISpatializedHearingAidSimulationFactory::HearingAidSimulation both_hs;
		both_hs.attack_ms = processing.attack_ms;
		both_hs.release_ms = processing.release_ms;
		both_hs.chunkSize = processing.chunkSize;
		both_hs.windowSize = processing.windowSize;
		both_hs.fullScaleLevel_dB_Spl = SpatialHearingAidModel::fullScaleLevel_dB_Spl;

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

		auto computer = calibrationComputerFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - SpatialHearingAidModel::fullScaleLevel_dB_Spl;

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
	ICalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoNoSimulation(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory
	) noexcept :
		simulationFactory{ simulationFactory },
		calibrationComputerFactory{ calibrationComputerFactory } {}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *reader, double level_dB_Spl) override {
		return std::make_shared<ChannelProcessingGroup>(makeChannels(reader, level_dB_Spl));
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> makeChannels(
		AudioFrameReader *reader, 
		double level_dB_Spl
	) {
		auto computer = calibrationComputerFactory->make(reader);
		const auto digitalLevel = level_dB_Spl - SpatialHearingAidModel::fullScaleLevel_dB_Spl;
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
	ICalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoProcessorFactoryFactory(
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory
	) noexcept :
		simulationFactory{ simulationFactory },
		calibrationComputerFactory{ calibrationComputerFactory } {}

	std::shared_ptr<AudioFrameProcessorFactory> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse brir
	) override {
		return std::make_shared<StereoSpatializationFactory>(
			std::move(brir), 
			simulationFactory, 
			calibrationComputerFactory
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
			calibrationComputerFactory
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
			calibrationComputerFactory
		);
	}

	std::shared_ptr<AudioFrameProcessorFactory> makeNoSimulation() override {
		return std::make_shared<StereoNoSimulation>(
			simulationFactory, 
			calibrationComputerFactory
		);
	}
};

// The MATLAB hearing aid simulation uses 119 dB SPL as a "max"
double const SpatialHearingAidModel::fullScaleLevel_dB_Spl = 119;
int const SpatialHearingAidModel::defaultFramesPerBuffer = 1024;

SpatialHearingAidModel::SpatialHearingAidModel(
	StimulusList *stimulusList,
	Documenter *documenter,
	AudioPlayer *player,
	AudioProcessingLoaderFactory *audioLoaderFactory,
	AudioFrameReaderFactory *audioReaderFactory,
	AudioFrameWriterFactory *audioWriterFactory,
	PrescriptionReader *prescriptionReader,
	BrirReader *brirReader,
	ISpatializedHearingAidSimulationFactory *simulationFactory,
	ICalibrationComputerFactory *calibrationComputerFactory
) :
	stimulusList{ stimulusList },
	documenter{ documenter },
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	audioReaderFactory{ audioReaderFactory },
	audioWriterFactory{ audioWriterFactory },
	player{ player },
	audioProcessingLoaderFactory{ audioLoaderFactory },
	processorFactoryFactory{
		std::make_shared<StereoProcessorFactoryFactory>(
			simulationFactory, 
			calibrationComputerFactory
		)
	},
	processorFactoryForTest{
		std::make_shared<NullProcessorFactory>()
	}
{
}

void SpatialHearingAidModel::prepareNewTest(Testing *p) {
	framesPerBufferForTest = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	processorFactoryForTest = makeProcessorFactory(p->processing);
	prepareNewTest_(p);
}

std::shared_ptr<AudioFrameProcessorFactory> SpatialHearingAidModel::makeProcessorFactory(
	SignalProcessing p
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

BrirReader::BinauralRoomImpulseResponse SpatialHearingAidModel::readAndCheckBrir(std::string filePath) {
	auto brir = readBrir(std::move(filePath));
	if (brir.left.empty())
		throw RequestFailure{ coefficientErrorMessage("left") };
	if (brir.right.empty())
		throw RequestFailure{ coefficientErrorMessage("right") };
	return brir;
}

BrirReader::BinauralRoomImpulseResponse SpatialHearingAidModel::readBrir(std::string filePath) {
	try {
		return brirReader->read(filePath);
	}
	catch (const BrirReader::ReadFailure &) {
		throw RequestFailure{ "BRIR '" + filePath + "' cannot be read." };
	}
}

PrescriptionReader::Dsl SpatialHearingAidModel::readPrescription(std::string filePath) {
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

void SpatialHearingAidModel::assertSizeIsPowerOfTwo(int size) {
	if (!powerOfTwo(size))
		throw RequestFailure{ windowChunkSizesErrorMessage(size) };
}

void SpatialHearingAidModel::prepareNewTest_(Testing *p) {
	try {
		stimulusList->initialize(p->audioDirectory);
		documenter->initialize(p->testFilePath);
		documenter->documentTestParameters(p);
		nextStimulus_ = stimulusList->next();
	}
	catch (const std::runtime_error &e) {
		throw RequestFailure{ e.what() };
	}
}

void SpatialHearingAidModel::playNextTrial(Trial *p) {
	if (player->isPlaying())
		return;

	PlayAudioRequest request;
	request.audioFilePath = nextStimulus_;
	request.audioDevice = p->audioDevice;
	request.level_dB_Spl = p->level_dB_Spl;
	request.framesPerBuffer = framesPerBufferForTest;
	request.processorFactory = processorFactoryForTest.get();
	playAudio(&request);
	Documenter::TrialParameters trial;
	trial.level_dB_Spl = p->level_dB_Spl;
	trial.stimulus = nextStimulus_;
	documenter->documentTrialParameters(std::move(trial));
	nextStimulus_ = stimulusList->next();
}

void SpatialHearingAidModel::playAudio(PlayAudioRequest *p) {
	auto reader = makeReader(p->audioFilePath);
	player->setAudioLoader(audioProcessingLoaderFactory->make(
		reader, 
		p->processorFactory->make(reader.get(), p->level_dB_Spl))
	);
	AudioPlayer::Preparation playing;
	playing.channels = reader->channels();
	playing.sampleRate = reader->sampleRate();
	playing.framesPerBuffer = p->framesPerBuffer;
	playing.audioDevice = p->audioDevice;
	prepareAudioPlayer(std::move(playing));
	player->play();
}

std::shared_ptr<AudioFrameReader> SpatialHearingAidModel::makeReader(std::string filePath) {
	try {
		return audioReaderFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

void SpatialHearingAidModel::prepareAudioPlayer(AudioPlayer::Preparation p) {
	try {
		player->prepareToPlay(std::move(p));
	}
	catch (const AudioPlayer::PreparationFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

void SpatialHearingAidModel::playCalibration(Calibration *p) {
	if (player->isPlaying())
		return;

	const auto framesPerBuffer = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	auto processorFactory_ = makeProcessorFactory(p->processing);

	PlayAudioRequest request;
	request.audioFilePath = p->audioFilePath;
	request.audioDevice = p->audioDevice;
	request.level_dB_Spl = p->level_dB_Spl;
	request.framesPerBuffer = framesPerBuffer;
	request.processorFactory = processorFactory_.get();
	playAudio(&request);
}

void SpatialHearingAidModel::stopCalibration() {
	player->stop();
}

void SpatialHearingAidModel::processAudioForSaving(SavingAudio *p) {
	auto reader = makeReader(p->inputAudioFilePath);
	auto processorFactory_ = makeProcessorFactory(p->processing);
	auto processor_ = processorFactory_->make(reader.get(), p->level_dB_Spl);
	auto loader_ = audioProcessingLoaderFactory->make(reader, {});
	using channel_type = AudioProcessingLoader::channel_type;
	std::vector<std::vector<channel_type::element_type>> channels(reader->channels());
	std::vector<channel_type> adapted;
	const auto framesPerBuffer = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	for (auto &channel : channels) {
		channel.resize(framesPerBuffer);
		adapted.push_back({ channel });
	}

	while (!loader_->complete())
		loader_->load(adapted);
}

void SpatialHearingAidModel::saveAudio(std::string filePath) {
	auto writer_ = makeWriter(std::move(filePath));
}

std::shared_ptr<AudioFrameWriter> SpatialHearingAidModel::makeWriter(std::string filePath) {
	try {
		return audioWriterFactory->make(std::move(filePath));
	}
	catch (const AudioFrameWriterFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

bool SpatialHearingAidModel::testComplete() {
	return stimulusList->empty();
}

std::vector<std::string> SpatialHearingAidModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
