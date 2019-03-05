#include "SpatialHearingAidModel.h"
#include "ChannelProcessingGroup.h"
#include <gsl/gsl>

class StereoCalibration {
	std::shared_ptr<CalibrationComputer> computer;
	double digitalLevel;
public:
	StereoCalibration(
		std::shared_ptr<CalibrationComputer> computer, 
		double level_dB_Spl
	) :
		computer{ std::move(computer) },
		digitalLevel{ level_dB_Spl - SpatialHearingAidModel::fullScaleLevel_dB_Spl }
	{}

	float leftChannelScale() {
		return gsl::narrow_cast<float>(channelScale(0));
	}

	double channelScale(int channel) {
		return computer->signalScale(channel, digitalLevel);
	}

	float rightChannelScale() {
		return gsl::narrow_cast<float>(channelScale(1));
	}
};

class NullProcessorFactory : public StereoSimulationFactory {
	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *, double) override { return {}; }
};

class StereoSpatializationFactory : public StereoSimulationFactory {
	SimulationChannelFactory::Spatialization left_spatial;
	SimulationChannelFactory::Spatialization right_spatial;
	SimulationChannelFactory *channelFactory;
	CalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoSpatializationFactory(
		BrirReader::BinauralRoomImpulseResponse brir_,
		SimulationChannelFactory *channelFactory,
		CalibrationComputerFactory *calibrationComputerFactory
	) :
		channelFactory{ channelFactory },
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
		StereoCalibration stereoCalibration{ calibrationComputerFactory->make(reader), level_dB_Spl };

		return { 
			channelFactory->makeSpatialization(
				left_spatial, 
				stereoCalibration.leftChannelScale()
			), 
			channelFactory->makeSpatialization(
				right_spatial, 
				stereoCalibration.rightChannelScale()
			) 
		};
	}
};

class StereoHearingAidFactory : public StereoSimulationFactory {
	SimulationChannelFactory::HearingAidSimulation left_hs;
	SimulationChannelFactory::HearingAidSimulation right_hs;
	SimulationChannelFactory *channelFactory;
	CalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoHearingAidFactory(
		HearingAidSimulation processing,
		SimulationChannelFactory *channelFactory,
		CalibrationComputerFactory *calibrationComputerFactory
	) :
		channelFactory{ channelFactory },
		calibrationComputerFactory{ calibrationComputerFactory } 
	{
		SimulationChannelFactory::HearingAidSimulation both_hs;
		both_hs.attack_ms = processing.attack_ms;
		both_hs.release_ms = processing.release_ms;
		both_hs.chunkSize = processing.chunkSize;
		both_hs.windowSize = processing.windowSize;
		both_hs.fullScaleLevel_dB_Spl = SpatialHearingAidModel::fullScaleLevel_dB_Spl;

		left_hs = both_hs;
		right_hs = both_hs;
		left_hs.prescription = std::move(processing.leftPrescription);
		right_hs.prescription = std::move(processing.rightPrescription);
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

		StereoCalibration stereoCalibration{ calibrationComputerFactory->make(reader), level_dB_Spl };

		return { 
			channelFactory->makeHearingAidSimulation(
				left_hs, 
				stereoCalibration.leftChannelScale()
			), 
			channelFactory->makeHearingAidSimulation(
				right_hs, 
				stereoCalibration.rightChannelScale()
			) 
		};
	}
};

class StereoSpatializedHearingAidSimulationFactory : public StereoSimulationFactory {	
	SimulationChannelFactory::FullSimulation left_fs;	
	SimulationChannelFactory::FullSimulation right_fs;
	SimulationChannelFactory *channelFactory;
	CalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoSpatializedHearingAidSimulationFactory(
		BrirReader::BinauralRoomImpulseResponse brir_,
		StereoSimulationFactory::HearingAidSimulation processing,
		SimulationChannelFactory *channelFactory,
		CalibrationComputerFactory *calibrationComputerFactory
	) :
		channelFactory{ channelFactory },
		calibrationComputerFactory{ calibrationComputerFactory } 
	{
		left_fs.spatialization.filterCoefficients = std::move(brir_.left);
		right_fs.spatialization.filterCoefficients = std::move(brir_.right);

		SimulationChannelFactory::HearingAidSimulation both_hs;
		both_hs.attack_ms = processing.attack_ms;
		both_hs.release_ms = processing.release_ms;
		both_hs.chunkSize = processing.chunkSize;
		both_hs.windowSize = processing.windowSize;
		both_hs.fullScaleLevel_dB_Spl = SpatialHearingAidModel::fullScaleLevel_dB_Spl;

		left_fs.hearingAid = both_hs;
		right_fs.hearingAid = both_hs;
		left_fs.hearingAid.prescription = std::move(processing.leftPrescription);
		right_fs.hearingAid.prescription = std::move(processing.rightPrescription);
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
		
		StereoCalibration stereoCalibration{ calibrationComputerFactory->make(reader), level_dB_Spl };

		return { 
			channelFactory->makeFullSimulation(
				left_fs, 
				stereoCalibration.leftChannelScale()
			), 
			channelFactory->makeFullSimulation(
				right_fs, 
				stereoCalibration.rightChannelScale()
			) 
		};
	}
};

class StereoNoSimulation : public StereoSimulationFactory {
	SimulationChannelFactory *channelFactory;
	CalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoNoSimulation(
		SimulationChannelFactory *channelFactory,
		CalibrationComputerFactory *calibrationComputerFactory
	) noexcept :
		channelFactory{ channelFactory },
		calibrationComputerFactory{ calibrationComputerFactory } {}

	std::shared_ptr<AudioFrameProcessor> make(AudioFrameReader *reader, double level_dB_Spl) override {
		return std::make_shared<ChannelProcessingGroup>(makeChannels(reader, level_dB_Spl));
	}

	std::vector<ChannelProcessingGroup::channel_processing_type> makeChannels(
		AudioFrameReader *reader, 
		double level_dB_Spl
	) {
		StereoCalibration stereoCalibration{ calibrationComputerFactory->make(reader), level_dB_Spl };

		return { 
			channelFactory->makeWithoutSimulation(
				stereoCalibration.leftChannelScale()
			), 
			channelFactory->makeWithoutSimulation(
				stereoCalibration.rightChannelScale()
			)
		};
	}
};

class StereoProcessorFactoryFactory : public AudioFrameProcessorFactoryFactory {
	SimulationChannelFactory *channelFactory;
	CalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoProcessorFactoryFactory(
		SimulationChannelFactory *channelFactory,
		CalibrationComputerFactory *calibrationComputerFactory
	) noexcept :
		channelFactory{ channelFactory },
		calibrationComputerFactory{ calibrationComputerFactory } {}

	std::shared_ptr<StereoSimulationFactory> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse hearingAid
	) override {
		return std::make_shared<StereoSpatializationFactory>(
			std::move(hearingAid), 
			channelFactory, 
			calibrationComputerFactory
		);
	}
	
	std::shared_ptr<StereoSimulationFactory> makeHearingAid(
		StereoSimulationFactory::HearingAidSimulation hearingAid
	) override {
		return std::make_shared<StereoHearingAidFactory>(
			std::move(hearingAid),
			channelFactory, 
			calibrationComputerFactory
		);
	}

	std::shared_ptr<StereoSimulationFactory> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse brir, 
		StereoSimulationFactory::HearingAidSimulation hearingAid
	) override {
		return std::make_shared<StereoSpatializedHearingAidSimulationFactory>(
			std::move(brir), 
			std::move(hearingAid),
			channelFactory, 
			calibrationComputerFactory
		);
	}

	std::shared_ptr<StereoSimulationFactory> makeNoSimulation() override {
		return std::make_shared<StereoNoSimulation>(
			channelFactory, 
			calibrationComputerFactory
		);
	}
};

// The MATLAB hearing aid simulation uses 119 dB SPL as a "max"
double const SpatialHearingAidModel::fullScaleLevel_dB_Spl = 119;
int const SpatialHearingAidModel::defaultFramesPerBuffer = 1024;

SpatialHearingAidModel::SpatialHearingAidModel(
	StimulusList *stimulusList,
	TestDocumenter *documenter,
	AudioPlayer *player,
	AudioProcessingLoaderFactory *audioLoaderFactory,
	AudioFrameReaderFactory *audioReaderFactory,
	AudioFrameWriterFactory *audioWriterFactory,
	PrescriptionReader *prescriptionReader,
	BrirReader *brirReader,
	SimulationChannelFactory *channelFactory,
	CalibrationComputerFactory *calibrationComputerFactory
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
			channelFactory, 
			calibrationComputerFactory
		)
	},
	processorFactoryForTest{
		std::make_shared<NullProcessorFactory>()
	}
{
}

void SpatialHearingAidModel::prepareNewTest(const Testing &p) {
	framesPerBufferForTest = framesPerBuffer(p.processing);
	processorFactoryForTest = makeProcessorFactory(p.processing);
	prepareNewTest_(p);
}

int SpatialHearingAidModel::framesPerBuffer(const SignalProcessing &p) {
	return 
		p.usingHearingAidSimulation
		? p.chunkSize
		: defaultFramesPerBuffer;
}

std::shared_ptr<StereoSimulationFactory> SpatialHearingAidModel::makeProcessorFactory(
	const SignalProcessing &p
) {
	if (p.usingHearingAidSimulation && p.usingSpatialization)
		return processorFactoryFactory->makeFullSimulation(
			readAndCheckBrir(std::move(p.brirFilePath)),
			hearingAidSimulation(p)
		);
	else if (p.usingSpatialization)
		return processorFactoryFactory->makeSpatialization(
			readAndCheckBrir(std::move(p.brirFilePath))
		);
	else if (p.usingHearingAidSimulation)
		return processorFactoryFactory->makeHearingAid(hearingAidSimulation(p));
	else
		return processorFactoryFactory->makeNoSimulation();
}

StereoSimulationFactory::HearingAidSimulation 
	SpatialHearingAidModel::hearingAidSimulation(const SignalProcessing &p) 
{
	assertSizeIsPowerOfTwo(p.chunkSize);
	assertSizeIsPowerOfTwo(p.windowSize);
	StereoSimulationFactory::HearingAidSimulation simulation;
	simulation.attack_ms = p.attack_ms;
	simulation.release_ms = p.release_ms;
	simulation.chunkSize = p.chunkSize;
	simulation.windowSize = p.windowSize;
	simulation.leftPrescription = readPrescription(std::move(p.leftDslPrescriptionFilePath));
	simulation.rightPrescription = readPrescription(std::move(p.rightDslPrescriptionFilePath));
	return simulation;
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

void SpatialHearingAidModel::prepareNewTest_(const Testing &p) {
	try {
		stimulusList->initialize(p.audioDirectory);
		documenter->initialize(p.testFilePath);
		documenter->documentTestParameters(p);
		nextStimulus_ = stimulusList->next();
	}
	catch (const TestDocumenter::InitializationFailure &) {
		throw RequestFailure{ "Test file '" + p.testFilePath + "' cannot be written." };
	}
	catch (const StimulusList::InitializationFailure &) {
		throw RequestFailure{ "Stimulus list '" + p.audioDirectory + "' cannot be read." };
	}
}

void SpatialHearingAidModel::playNextTrial(const Trial &p) {
	if (player->isPlaying())
		return;

	PlayAudioRequest request;
	request.audioFilePath = nextStimulus_;
	request.audioDevice = std::move(p.audioDevice);
	request.level_dB_Spl = p.level_dB_Spl;
	request.framesPerBuffer = framesPerBufferForTest;
	request.processorFactory = processorFactoryForTest.get();
	playAudio(request);
	TestDocumenter::TrialParameters trial;
	trial.level_dB_Spl = p.level_dB_Spl;
	trial.stimulus = nextStimulus_;
	documenter->documentTrialParameters(std::move(trial));
	nextStimulus_ = stimulusList->next();
}

void SpatialHearingAidModel::playAudio(const PlayAudioRequest &p) {
	auto reader = makeReader(p.audioFilePath);

	AudioPlayer::Preparation preparation;
	preparation.channels = reader->channels();
	preparation.sampleRate = reader->sampleRate();
	preparation.framesPerBuffer = p.framesPerBuffer;
	preparation.audioDevice = std::move(p.audioDevice);
	prepareAudioPlayer(preparation);

	MakeAudioLoader makingLoader;
	makingLoader.level_dB_Spl = p.level_dB_Spl;
	makingLoader.reader = std::move(reader);
	makingLoader.processorFactory = p.processorFactory;
	player->setAudioLoader(makeLoader(makingLoader));

	player->play();
}

std::shared_ptr<AudioLoader> SpatialHearingAidModel::makeLoader(const MakeAudioLoader &p) {
	return audioProcessingLoaderFactory->make(
		p.reader, 
		p.processorFactory->make(p.reader.get(), p.level_dB_Spl)
	);
}

std::shared_ptr<AudioFrameReader> SpatialHearingAidModel::makeReader(std::string filePath) {
	try {
		return audioReaderFactory->make(filePath);
	}
	catch (const AudioFrameReaderFactory::CreateError &) {
		throw RequestFailure{ "Audio file '" + filePath + "' cannot be read." };
	}
}

void SpatialHearingAidModel::prepareAudioPlayer(const AudioPlayer::Preparation &p) {
	try {
		player->prepareToPlay(p);
	}
	catch (const AudioPlayer::PreparationFailure &) {
		throw RequestFailure{ "Audio device '" + p.audioDevice + "' cannot be opened." };
	}
}

void SpatialHearingAidModel::playCalibration(const Calibration &p) {
	if (player->isPlaying())
		return;

	const auto framesPerBuffer_ = framesPerBuffer(p.processing);
	auto processorFactory_ = makeProcessorFactory(p.processing);

	PlayAudioRequest request;
	request.audioFilePath = std::move(p.audioFilePath);
	request.audioDevice = std::move(p.audioDevice);
	request.level_dB_Spl = p.level_dB_Spl;
	request.framesPerBuffer = framesPerBuffer_;
	request.processorFactory = processorFactory_.get();
	playAudio(request);
}

void SpatialHearingAidModel::stopCalibration() {
	player->stop();
}

void SpatialHearingAidModel::processAudioForSaving(const SavingAudio &p) {
	auto reader = makeReader(p.inputAudioFilePath);
	auto processorFactory_ = makeProcessorFactory(p.processing);
	MakeAudioLoader loading;
	loading.level_dB_Spl = p.level_dB_Spl;
	loading.reader = reader;
	loading.processorFactory = processorFactory_.get();
	auto loader_ = makeLoader(loading);
	const auto framesPerBuffer_ = framesPerBuffer(p.processing);
	using channel_type = AudioLoader::channel_type;
	std::vector<std::vector<channel_type::element_type>> channels(reader->channels());
	std::vector<channel_type> adapted;
	for (auto &channel : channels) {
		channel.resize(framesPerBuffer_);
		adapted.push_back({ channel });
	}
	toWrite_.resize(reader->channels());
	for (auto &channel : toWrite_)
		channel.clear();

	while (!loader_->complete()) {
		loader_->load(adapted);
		for (int i = 0; i < reader->channels(); ++i)
			toWrite_.at(i).insert(
				toWrite_.at(i).end(), 
				channels.at(i).begin(), 
				channels.at(i).end()
			);
	}
}

void SpatialHearingAidModel::saveAudio(std::string filePath) {
	auto writer_ = makeWriter(std::move(filePath));
	using channel_type = AudioFrameWriter::channel_type;
	std::vector<channel_type> adapted;
	for (auto &channel : toWrite_)
		adapted.push_back({ channel });
	writer_->write(adapted);
}

std::shared_ptr<AudioFrameWriter> SpatialHearingAidModel::makeWriter(std::string filePath) {
	try {
		return audioWriterFactory->make(filePath);
	}
	catch (const AudioFrameWriterFactory::CreateError &) {
		throw RequestFailure{ "Audio file '" + filePath + "' cannot be written." };
	}
}

bool SpatialHearingAidModel::testComplete() {
	return stimulusList->empty();
}

std::vector<std::string> SpatialHearingAidModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
