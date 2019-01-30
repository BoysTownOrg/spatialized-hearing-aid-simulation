#include "RefactoredModel.h"

void RefactoredModel::prepareNewTest(TestParameters p) {
	prepareNewTest_(p);
	if (p.usingSpatialization)
		readBrir(p);
	if (p.usingHearingAidSimulation)
		readPrescriptions(p);
	testParameters = p;
}

void RefactoredModel::prepareNewTest_(Model::TestParameters p) {
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

BrirReader::BinauralRoomImpulseResponse RefactoredModel::readBrir(Model::TestParameters p) {
	try {
		return brirReader->read(p.brirFilePath);
	}
	catch (const BrirReader::ReadFailure &) {
		throw TestInitializationFailure{ "Unable to read '" + p.brirFilePath + "'." };
	}
}

void RefactoredModel::readPrescriptions(Model::TestParameters p) {
	readPrescription(p.leftDslPrescriptionFilePath);
	readPrescription(p.rightDslPrescriptionFilePath);
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
	loader->setReader(reader);
	prepareAudioPlayer(*reader, p);
	test->playNextTrial(player);
	makeCompressor(
		prescriptionReader->read(testParameters.leftDslPrescriptionFilePath),
		reader->sampleRate()
	);
	makeCompressor(
		prescriptionReader->read(testParameters.rightDslPrescriptionFilePath),
		reader->sampleRate()
	);
	reader->reset();
}

void RefactoredModel::prepareAudioPlayer(AudioFrameReader & reader, Model::TrialParameters p) {
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

std::shared_ptr<FilterbankCompressor> RefactoredModel::makeCompressor(
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
	return compressorFactory->make(compression);
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
