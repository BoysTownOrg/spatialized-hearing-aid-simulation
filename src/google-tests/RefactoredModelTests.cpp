#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <presentation/Model.h>

class AudioProcessor {
public:
	INTERFACE_OPERATIONS(AudioProcessor);
};

class SpeechPerceptionTest {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest);
	struct TestParameters {
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	virtual void playTrial() = 0;
	virtual std::string nextStimulus() = 0;
};

class RefactoredModel : public Model {
	TestParameters testParameters{};
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	SpeechPerceptionTest *test;
	AudioProcessor *processor;
	FilterbankCompressorFactory *compressorFactory;
	AudioFrameReaderFactory *readerFactory;
public:
	RefactoredModel(
		SpeechPerceptionTest *test,
		AudioProcessor *processor,
		PrescriptionReader *prescriptionReader,
		BrirReader *brirReader,
		FilterbankCompressorFactory *compressorFactory,
		AudioFrameReaderFactory *readerFactory
	) :
		prescriptionReader{ prescriptionReader },
		brirReader{ brirReader },
		test{ test },
		processor{ processor },
		compressorFactory{ compressorFactory },
		readerFactory{ readerFactory } {}

	void prepareNewTest(TestParameters p) override {
		testParameters = p;
		SpeechPerceptionTest::TestParameters adapted;
		adapted.audioDirectory = p.audioDirectory;
		adapted.testFilePath = p.testFilePath;
		test->prepareNewTest(adapted);
		if (p.usingSpatialization)
			brirReader->read(p.brirFilePath);
		if (p.usingHearingAidSimulation)
			readPrescriptions(p);
	}

	void playTrial(TrialParameters p) override {
		test->playTrial();
		auto reader = readerFactory->make(test->nextStimulus());
		makeCompressor(prescriptionReader->read(testParameters.leftDslPrescriptionFilePath), reader->sampleRate());
		makeCompressor(prescriptionReader->read(testParameters.rightDslPrescriptionFilePath), reader->sampleRate());
	}

private:
	void makeCompressor(PrescriptionReader::Dsl dsl, int sampleRate) {
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
		compressorFactory->make(compression);
	}

public:
	bool testComplete() override {
		return false;
	}

	void playCalibration(CalibrationParameters) override {
	}

	void stopCalibration() override {
	}

private:
	void readPrescriptions(Model::TestParameters p) {
		try {
			prescriptionReader->read(p.leftDslPrescriptionFilePath);
			prescriptionReader->read(p.rightDslPrescriptionFilePath);
		}
		catch (const PrescriptionReader::ReadFailure &) {
			throw TestInitializationFailure{ "Unable to read '" + p.leftDslPrescriptionFilePath + "'." };
		}
	}
};

#include "ArgumentCollection.h"
#include <map>

class PrescriptionReaderStub : public PrescriptionReader {
	ArgumentCollection<std::string> filePaths_{};
	std::map<std::string, Dsl> prescriptions_{};
public:
	void addPrescription(std::string filePath, Dsl dsl) {
		prescriptions_[filePath] = dsl;
	}

	Dsl read(std::string filePath) override {
		filePaths_.push_back(filePath);
		return prescriptions_[filePath];
	}

	ArgumentCollection<std::string> filePaths() const {
		return filePaths_;
	}
};

class FailingPrescriptionReader : public PrescriptionReader {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	Dsl read(std::string) override {
		throw ReadFailure{ errorMessage };
	}
};

class BrirReaderStub : public BrirReader {
	std::string filePath_{};
public:
	BinauralRoomImpulseResponse read(std::string filePath) override {
		filePath_ = std::move(filePath);
		return {};
	}

	std::string filePath() const {
		return filePath_;
	}
};

class SpeechPerceptionTestStub : public SpeechPerceptionTest {
	TestParameters testParameters_{};
	std::string trialLog_{};
	std::string nextStimulus_{};
public:
	const TestParameters &testParameters() const {
		return testParameters_;
	}

	void prepareNewTest(TestParameters p) override {
		testParameters_ = std::move(p);
	}

	void playTrial() override {
		trialLog_ += "playTrial ";
	}

	std::string trialLog() const {
		return trialLog_;
	}

	void setNextStimulus(std::string s) {
		nextStimulus_ = std::move(s);
	}

	std::string nextStimulus() override {
		return nextStimulus_;
	}
};

class AudioProcessorStub : public AudioProcessor {
};

#include "assert-utility.h"
#include "FilterbankCompressorSpy.h"
#include "AudioFrameReaderStub.h"
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	RefactoredModel::TrialParameters trial{};
	PrescriptionReaderStub prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	AudioProcessorStub processor{};
	FilterbankCompressorSpyFactory compressorFactory{};
	std::shared_ptr<AudioFrameReaderStub> reader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory readerFactory{reader};
	RefactoredModel model{ 
		&test, 
		&processor, 
		&prescriptionReader, 
		&brirReader, 
		&compressorFactory, 
		&readerFactory 
	};

	void prepareNewTest() {
		model.prepareNewTest(testing);
	}

	void playTrial() {
		model.playTrial(trial);
	}
};

TEST_F(RefactoredModelTests, prepareNewTestReadsPrescriptionsWhenUsingHearingAidSimulation) {
	testing.usingHearingAidSimulation = true;
	testing.leftDslPrescriptionFilePath = "a";
	testing.rightDslPrescriptionFilePath = "b";
	prepareNewTest();
	EXPECT_TRUE(prescriptionReader.filePaths().contains("a"));
	EXPECT_TRUE(prescriptionReader.filePaths().contains("b"));
}

TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation) {
	testing.usingHearingAidSimulation = false;
	testing.leftDslPrescriptionFilePath = "a";
	testing.rightDslPrescriptionFilePath = "b";
	prepareNewTest();
	EXPECT_TRUE(prescriptionReader.filePaths().empty());
}

TEST_F(RefactoredModelTests, prepareNewTestReadsBrirWhenUsingSpatialization) {
	testing.usingSpatialization = true;
	testing.brirFilePath = "a";
	prepareNewTest();
	assertEqual("a", brirReader.filePath());
}

TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
	testing.usingSpatialization = false;
	testing.brirFilePath = "a";
	prepareNewTest();
	EXPECT_TRUE(brirReader.filePath().empty());
}

TEST_F(RefactoredModelTests, prepareNewTestPassesParametersToSpeechPerceptionTest) {
	testing.audioDirectory = "a";
	testing.testFilePath = "b";
	prepareNewTest();
	assertEqual("a", test.testParameters().audioDirectory);
	assertEqual("b", test.testParameters().testFilePath);
}

TEST_F(RefactoredModelTests, DISABLED_playTrialPassesStimulusPlayerToSpeechPerceptionTest) {
	playTrial();
	//EXPECT_EQ(&player, test.stimulusPlayer());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioFilePathToFactory) {
	test.setNextStimulus("a");
	playTrial();
	assertEqual("a", readerFactory.filePath());
}

TEST_F(RefactoredModelTests, playTrialPassesCompressionParametersToFactory) {
	PrescriptionReader::Dsl leftPrescription;
	leftPrescription.compressionRatios = { 1 };
	leftPrescription.crossFrequenciesHz = { 2 };
	leftPrescription.kneepointGains_dB = { 3 };
	leftPrescription.kneepoints_dBSpl = { 4 };
	leftPrescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
	leftPrescription.channels = 6;
	prescriptionReader.addPrescription("leftFilePath", leftPrescription);
	PrescriptionReader::Dsl rightPrescription;
	rightPrescription.compressionRatios = { 1, 1 };
	rightPrescription.crossFrequenciesHz = { 2, 2 };
	rightPrescription.kneepointGains_dB = { 3, 3 };
	rightPrescription.kneepoints_dBSpl = { 4, 4 };
	rightPrescription.broadbandOutputLimitingThresholds_dBSpl = { 5, 5 };
	rightPrescription.channels = 12;
	prescriptionReader.addPrescription("rightFilePath", rightPrescription);
	reader->setSampleRate(7);
	testing.usingHearingAidSimulation = true;
	testing.attack_ms = 8;
	testing.release_ms = 9;
	testing.chunkSize = 10;
	testing.windowSize = 11;
	testing.leftDslPrescriptionFilePath = "leftFilePath";
	testing.rightDslPrescriptionFilePath = "rightFilePath";
	prepareNewTest();
	playTrial();
	auto left = compressorFactory.parameters().at(0);
	assertEqual({ 1 }, left.compressionRatios);
	assertEqual({ 2 }, left.crossFrequenciesHz);
	assertEqual({ 3 }, left.kneepointGains_dB);
	assertEqual({ 4 }, left.kneepoints_dBSpl);
	assertEqual({ 5 }, left.broadbandOutputLimitingThresholds_dBSpl);
	EXPECT_EQ(6, left.channels);
	EXPECT_EQ(7, left.sampleRate);
	EXPECT_EQ(8, left.attack_ms);
	EXPECT_EQ(9, left.release_ms);
	EXPECT_EQ(10, left.chunkSize);
	EXPECT_EQ(11, left.windowSize);
	auto right = compressorFactory.parameters().at(1);
	assertEqual({ 1, 1 }, right.compressionRatios);
	assertEqual({ 2, 2 }, right.crossFrequenciesHz);
	assertEqual({ 3, 3 }, right.kneepointGains_dB);
	assertEqual({ 4, 4 }, right.kneepoints_dBSpl);
	assertEqual({ 5, 5 }, right.broadbandOutputLimitingThresholds_dBSpl);
	EXPECT_EQ(12, right.channels);
	EXPECT_EQ(7, right.sampleRate);
	EXPECT_EQ(8, right.attack_ms);
	EXPECT_EQ(9, right.release_ms);
	EXPECT_EQ(10, right.chunkSize);
	EXPECT_EQ(11, right.windowSize);
}

class RefactoredModelWithFailingPrescriptionReaderTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	FailingPrescriptionReader prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	AudioProcessorStub processor{};
	FilterbankCompressorSpyFactory compressorFactory{};
	AudioFrameReaderStubFactory readerFactory{};
	RefactoredModel model{ 
		&test, 
		&processor, 
		&prescriptionReader, 
		&brirReader, 
		&compressorFactory, 
		&readerFactory 
	};

	void prepareNewTest() {
		model.prepareNewTest(testing);
	}
};

TEST_F(
	RefactoredModelWithFailingPrescriptionReaderTests, 
	prepareNewTestThrowsTestInitializationFailureWhenUsingHearingAidSimulation
) {
	prescriptionReader.setErrorMessage("irrelevant");
	try {
		testing.usingHearingAidSimulation = true;
		testing.leftDslPrescriptionFilePath = "a";
		prepareNewTest();
		FAIL() << "Expected RefactoredModel::TestInitializationFailure.";
	}
	catch (const RefactoredModel::TestInitializationFailure & e) {
		assertEqual("Unable to read 'a'.", e.what());
	}
}