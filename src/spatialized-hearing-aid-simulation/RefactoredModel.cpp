#include "RefactoredModel.h"

RefactoredModel::RefactoredModel(
	SpeechPerceptionTest *test,
	PrescriptionReader *prescriptionReader,
	BrirReader *brirReader,
	HearingAidFactory *hearingAidFactory,
	FirFilterFactory *firFilterFactory,
	AudioFrameReaderFactory *audioReaderFactory,
	AudioStimulusPlayer *player,
	AudioLoader *loader
) :
	prescriptionReader{ prescriptionReader },
	brirReader{ brirReader },
	test{ test },
	hearingAidFactory{ hearingAidFactory },
	firFilterFactory{ firFilterFactory },
	audioReaderFactory{ audioReaderFactory },
	player{ player },
	loader{ loader }
{
	player->setAudioLoader(loader);
}

static constexpr bool powerOfTwo(int n) noexcept {
	return n > 0 && (n & (n - 1)) == 0;
}

void RefactoredModel::prepareNewTest(TestParameters p) {
	prepareNewTest_(p);
	if (p.usingSpatialization) {
		brir = readBrir(p);
		if (brir.left.empty())
			throw TestInitializationFailure{"The left BRIR coefficients are empty, therefore a filter operation cannot be defined."};
		if (brir.right.empty())
			throw TestInitializationFailure{"The right BRIR coefficients are empty, therefore a filter operation cannot be defined."};
	}
	if (p.usingHearingAidSimulation) {
		readPrescriptions(p);
		if (!powerOfTwo(p.chunkSize))
			throw TestInitializationFailure{
				"Both the chunk size and window size must be powers of two; " + 
				std::to_string(p.chunkSize) + " is not a power of two."
			};
		if (!powerOfTwo(p.windowSize))
			throw TestInitializationFailure{
				"Both the chunk size and window size must be powers of two; " + 
				std::to_string(p.windowSize) + " is not a power of two."
			};
	}
	testParameters = p;
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

void RefactoredModel::playTrial(TrialParameters p) {
	auto reader = audioReaderFactory->make(test->nextStimulus());
	reader->reset();
	prepareAudioPlayer(*reader, p);
	test->playNextTrial(player);
	loader->setReader(reader);
	makeHearingAid(leftPrescription, reader->sampleRate());
	makeHearingAid(rightPrescription, reader->sampleRate());
	firFilterFactory->make(brir.left);
	firFilterFactory->make(brir.right);
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
