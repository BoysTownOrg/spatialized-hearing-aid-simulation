#include "SpatialHearingAidModel.h"
#include "ChannelProcessingGroup.h"
#include <gsl/gsl>

class StereoSimulationFactoryImpl : public StereoSimulationFactory {
	std::shared_ptr<ICalibrationComputer> computer;
	double digitalLevel;
	int sampleRate;
	SimulationChannelFactory *simulationFactory;
public:
	StereoSimulationFactoryImpl(
		SimulationChannelFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory,
		AudioFrameReader *reader, 
		double level_dB_Spl
	) :
		simulationFactory{ simulationFactory },
		computer{ calibrationComputerFactory->make(reader) },
		digitalLevel{ level_dB_Spl - SpatialHearingAidModel::fullScaleLevel_dB_Spl },
		sampleRate{ reader->sampleRate() }
	{}

	std::shared_ptr<AudioFrameProcessor> makeSpatialization(BrirReader::BinauralRoomImpulseResponse brir) override {
		SimulationChannelFactory::Spatialization left;
		left.filterCoefficients = std::move(brir.left);
		SimulationChannelFactory::Spatialization right;
		right.filterCoefficients = std::move(brir.right);
		std::vector<ChannelProcessingGroup::channel_processing_type> channels { 
			simulationFactory->makeSpatialization(
				left, 
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeSpatialization(
				right, 
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			) 
		};
		return std::make_shared<ChannelProcessingGroup>(channels);
	}

	std::shared_ptr<AudioFrameProcessor> makeHearingAid(HearingAidSimulation simulation) override {
		SimulationChannelFactory::HearingAidSimulation left;
		left.attack_ms = simulation.attack_ms;
		left.release_ms = simulation.release_ms;
		left.chunkSize = simulation.chunkSize;
		left.windowSize = simulation.windowSize;
		left.sampleRate = sampleRate;
		left.fullScaleLevel_dB_Spl = SpatialHearingAidModel::fullScaleLevel_dB_Spl;

		auto right = left;

		left.prescription = std::move(simulation.leftPrescription);
		right.prescription = std::move(simulation.rightPrescription);

		std::vector<ChannelProcessingGroup::channel_processing_type> channels { 
			simulationFactory->makeHearingAidSimulation(
				left, 
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeHearingAidSimulation(
				right, 
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			) 
		};
		return std::make_shared<ChannelProcessingGroup>(channels);
	}

	std::shared_ptr<AudioFrameProcessor> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse brir, 
		HearingAidSimulation simulation
	) override {
		SimulationChannelFactory::Spatialization left_sp;
		left_sp.filterCoefficients = std::move(brir.left);
		SimulationChannelFactory::Spatialization right_sp;
		right_sp.filterCoefficients = std::move(brir.right);

		SimulationChannelFactory::HearingAidSimulation left_ha;
		left_ha.attack_ms = simulation.attack_ms;
		left_ha.release_ms = simulation.release_ms;
		left_ha.chunkSize = simulation.chunkSize;
		left_ha.windowSize = simulation.windowSize;
		left_ha.sampleRate = sampleRate;
		left_ha.fullScaleLevel_dB_Spl = SpatialHearingAidModel::fullScaleLevel_dB_Spl;

		auto right_ha = left_ha;

		left_ha.prescription = std::move(simulation.leftPrescription);
		right_ha.prescription = std::move(simulation.rightPrescription);

		SimulationChannelFactory::FullSimulation left;
		SimulationChannelFactory::FullSimulation right;
		left.hearingAid = left_ha;
		left.spatialization = left_sp;
		right.hearingAid = right_ha;
		right.spatialization = right_sp;

		std::vector<ChannelProcessingGroup::channel_processing_type> channels { 
			simulationFactory->makeFullSimulation(
				left, 
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeFullSimulation(
				right, 
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			) 
		};
		return std::make_shared<ChannelProcessingGroup>(channels);
	}
	std::shared_ptr<AudioFrameProcessor> makeNoSimulation() override {
		std::vector<ChannelProcessingGroup::channel_processing_type> channels { 
			simulationFactory->makeWithoutSimulation(
				gsl::narrow_cast<float>(computer->signalScale(0, digitalLevel))
			), 
			simulationFactory->makeWithoutSimulation(
				gsl::narrow_cast<float>(computer->signalScale(1, digitalLevel))
			)
		};
		return std::make_shared<ChannelProcessingGroup>(channels);
	}
};

class StereoProcessorFactoryFactory : public AudioFrameProcessorFactoryFactory {
	SimulationChannelFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationComputerFactory;
public:
	StereoProcessorFactoryFactory(
		SimulationChannelFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationComputerFactory
	) noexcept :
		simulationFactory{ simulationFactory },
		calibrationComputerFactory{ calibrationComputerFactory } {}

	std::shared_ptr<StereoSimulationFactory> make(AudioFrameReader * reader, double level_dB_Spl) override {
		return std::make_shared<StereoSimulationFactoryImpl>(
			simulationFactory, 
			calibrationComputerFactory, 
			reader,
			level_dB_Spl
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
	SimulationChannelFactory *simulationFactory,
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
	}
{
}

void SpatialHearingAidModel::prepareNewTest(Testing *p) {
	framesPerBufferForTest = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	storeProcessing(p->processing);
	prepareNewTest_(p);
}

void SpatialHearingAidModel::storeProcessing(
	SignalProcessing p
) {
	usingHearingAidSimulation = p.usingHearingAidSimulation;
	usingSpatialization = p.usingSpatialization;
	if (p.usingHearingAidSimulation)
		hearingAidSimulation_ = hearingAidSimulation(p);
	if (p.usingSpatialization)
		brir = readAndCheckBrir(std::move(p.brirFilePath));
}

StereoSimulationFactory::HearingAidSimulation 
	SpatialHearingAidModel::hearingAidSimulation(SignalProcessing p) 
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
	auto brir_ = readBrir(std::move(filePath));
	if (brir_.left.empty())
		throw RequestFailure{ coefficientErrorMessage("left") };
	if (brir_.right.empty())
		throw RequestFailure{ coefficientErrorMessage("right") };
	return brir_;
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

	auto reader = makeReader(nextStimulus_);
	
	auto factory = processorFactoryFactory->make(reader.get(), p->level_dB_Spl);
	std::shared_ptr<AudioFrameProcessor> processor_{};
	if (usingHearingAidSimulation && usingSpatialization)
		processor_ = factory->makeFullSimulation(
			brir,
			hearingAidSimulation_
		);
	else if (usingSpatialization)
		processor_ = factory->makeSpatialization(
			brir
		);
	else if (usingHearingAidSimulation)
		processor_ = factory->makeHearingAid(hearingAidSimulation_);
	else
		processor_ = factory->makeNoSimulation();

	player->setAudioLoader(audioProcessingLoaderFactory->make(
		reader, 
		processor_)
	);
	AudioPlayer::Preparation playing;
	playing.channels = reader->channels();
	playing.sampleRate = reader->sampleRate();
	playing.framesPerBuffer = framesPerBufferForTest;
	playing.audioDevice = p->audioDevice;
	prepareAudioPlayer(std::move(playing));
	player->play();
	Documenter::TrialParameters trial;
	trial.level_dB_Spl = p->level_dB_Spl;
	trial.stimulus = nextStimulus_;
	documenter->documentTrialParameters(std::move(trial));
	nextStimulus_ = stimulusList->next();
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

	auto reader = makeReader(p->audioFilePath);

	auto factory = processorFactoryFactory->make(reader.get(), p->level_dB_Spl);
	std::shared_ptr<AudioFrameProcessor> processor_{};
	if (p->processing.usingHearingAidSimulation && p->processing.usingSpatialization)
		processor_ = factory->makeFullSimulation(
			readAndCheckBrir(std::move(p->processing.brirFilePath)),
			hearingAidSimulation(p->processing)
		);
	else if (p->processing.usingSpatialization)
		processor_ = factory->makeSpatialization(
			readAndCheckBrir(std::move(p->processing.brirFilePath))
		);
	else if (p->processing.usingHearingAidSimulation)
		processor_ = factory->makeHearingAid(hearingAidSimulation(p->processing));
	else
		processor_ = factory->makeNoSimulation();

	player->setAudioLoader(audioProcessingLoaderFactory->make(
		reader, 
		processor_)
	);
	AudioPlayer::Preparation playing;
	playing.channels = reader->channels();
	playing.sampleRate = reader->sampleRate();
	playing.framesPerBuffer = framesPerBuffer;
	playing.audioDevice = p->audioDevice;
	prepareAudioPlayer(std::move(playing));
	player->play();
}

void SpatialHearingAidModel::stopCalibration() {
	player->stop();
}

void SpatialHearingAidModel::processAudioForSaving(SavingAudio *p) {
	auto reader = makeReader(p->inputAudioFilePath);
	
	auto factory = processorFactoryFactory->make(reader.get(), p->level_dB_Spl);
	std::shared_ptr<AudioFrameProcessor> processor_{};
	if (p->processing.usingHearingAidSimulation && p->processing.usingSpatialization)
		processor_ = factory->makeFullSimulation(
			readAndCheckBrir(std::move(p->processing.brirFilePath)),
			hearingAidSimulation(p->processing)
		);
	else if (p->processing.usingSpatialization)
		processor_ = factory->makeSpatialization(
			readAndCheckBrir(std::move(p->processing.brirFilePath))
		);
	else if (p->processing.usingHearingAidSimulation)
		processor_ = factory->makeHearingAid(hearingAidSimulation(p->processing));
	else
		processor_ = factory->makeNoSimulation();
	auto loader_ = audioProcessingLoaderFactory->make(reader, processor_);
	const auto framesPerBuffer = p->processing.usingHearingAidSimulation
		? p->processing.chunkSize
		: defaultFramesPerBuffer;
	using channel_type = AudioProcessingLoader::channel_type;
	std::vector<std::vector<channel_type::element_type>> channels(reader->channels());
	std::vector<channel_type> adapted;
	for (auto &channel : channels) {
		channel.resize(framesPerBuffer);
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
