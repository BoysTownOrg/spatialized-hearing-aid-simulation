#include "RefactoredModel.h"
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ChannelProcessingGroup.h>
#include <gsl/gsl>

RefactoredModel::RefactoredModel(
	SpeechPerceptionTest *test,
	PrescriptionReader *prescriptionReader,
	BrirReader *brirReader,
	HearingAidFactory *hearingAidFactory,
	FirFilterFactory *firFilterFactory,
	ScalarFactory *scalarFactory,
	AudioFrameReaderFactory *audioReaderFactory,
	AudioStimulusPlayer *player,
	AudioLoader *loader
) :
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	test{ test },
	hearingAidFactory{ hearingAidFactory },
	firFilterFactory{ firFilterFactory },
	scalarFactory{ scalarFactory },
	audioReaderFactory{ audioReaderFactory },
	player{ player },
	loader{ loader }
{
	player->setAudioLoader(loader);
}

static constexpr bool powerOfTwo(int n) noexcept {
	return n > 0 && (n & (n - 1)) == 0;
}

static std::string coefficientErrorMessage(std::string which) {
	return 
		"The " + which + " BRIR coefficients are empty, "
		"therefore a filter operation cannot be defined.";
}

static std::string windowChunkSizesErrorMessage(int offender) {
	return
		"Both the chunk size and window size must be powers of two; " +
		std::to_string(offender) + " is not a power of two.";
}

void RefactoredModel::prepareNewTest(TestParameters p) {
	prepareNewTest_(p);
	if (p.usingSpatialization)
		checkAndStoreBrir(p);
	if (p.usingHearingAidSimulation)
		checkAndStorePrescriptions(p);
	testParameters = p;
}

void RefactoredModel::checkAndStorePrescriptions(TestParameters p) {
	readPrescriptions(p);
	if (!powerOfTwo(p.chunkSize))
		throw TestInitializationFailure{ windowChunkSizesErrorMessage(p.chunkSize) };
	if (!powerOfTwo(p.windowSize))
		throw TestInitializationFailure{ windowChunkSizesErrorMessage(p.windowSize) };
}

void RefactoredModel::checkAndStoreBrir(TestParameters p) {
	brir = readBrir(p);
	if (brir.left.empty())
		throw TestInitializationFailure{ coefficientErrorMessage("left") };
	if (brir.right.empty())
		throw TestInitializationFailure{ coefficientErrorMessage("right") };
}

void RefactoredModel::prepareNewTest_(TestParameters p) {
	SpeechPerceptionTest::TestParameters adapted;
	adapted.audioDirectory = p.audioDirectory;
	adapted.testFilePath = p.testFilePath;
	try {
		test->prepareNewTest(adapted);
	}
	catch (const SpeechPerceptionTest::TestInitializationFailure &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

BrirReader::BinauralRoomImpulseResponse RefactoredModel::readBrir(TestParameters p) {
	try {
		return brirReader->read(p.brirFilePath);
	}
	catch (const BrirReader::ReadFailure &) {
		throw TestInitializationFailure{ "Unable to read '" + p.brirFilePath + "'." };
	}
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

class RmsComputer {
    std::vector<std::vector<float>> entireAudioFile;
public:
	explicit RmsComputer(AudioFrameReader &reader) :
		entireAudioFile(
			reader.channels(), 
			std::vector<float>(gsl::narrow<std::vector<float>::size_type>(reader.frames()))
		)
	{
		std::vector<gsl::span<float>> pointers;
		for (auto &channel : entireAudioFile)
			pointers.push_back({ channel });
		reader.read(pointers);
		reader.reset();
	}

    float compute(int channel) {
		return rms(entireAudioFile.at(channel));
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
	std::vector<ChannelProcessingGroup::channel_processing_type> channels{};
	const auto leftChannel = std::make_shared<SignalProcessingChain>();
	const auto rightChannel = std::make_shared<SignalProcessingChain>();
	auto reader = audioReaderFactory->make(test->nextStimulus());
    RmsComputer rms{ *reader };
    const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - 8) / 20.0);
	if (reader->channels() > 0)
		leftChannel->add(scalarFactory->make(gsl::narrow_cast<float>(desiredRms / rms.compute(0))));
	if (reader->channels() > 1)
		rightChannel->add(scalarFactory->make(gsl::narrow_cast<float>(desiredRms / rms.compute(1))));
	prepareAudioPlayer(*reader, p);
	test->playNextTrial(player);
	loader->setReader(reader);
	leftChannel->add(firFilterFactory->make(brir.left));
	rightChannel->add(firFilterFactory->make(brir.right));
	leftChannel->add(makeHearingAid(leftPrescription, reader->sampleRate()));
	rightChannel->add(makeHearingAid(rightPrescription, reader->sampleRate()));
	channels.push_back(leftChannel);
	channels.push_back(rightChannel);
	loader->setProcessor(std::make_shared<ChannelProcessingGroup>(channels));
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
	compression.broadbandOutputLimitingThresholds_dBSpl = dsl.broadbandOutputLimitingThresholds_dBSpl;
	compression.channels = dsl.channels;
	return hearingAidFactory->make(compression);
}

bool RefactoredModel::testComplete() {
	return false;
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
