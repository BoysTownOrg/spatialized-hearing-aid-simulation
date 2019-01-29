#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <recognition-test/StimulusPlayer.h>
#include <presentation/Model.h>

class SpeechPerceptionTest {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest);
	struct TestParameters {
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	virtual void playNextTrial(StimulusPlayer *) = 0;
	virtual std::string nextStimulus() = 0;
};

class RefactoredModel : public Model {
	TestParameters testParameters{};
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	SpeechPerceptionTest *test;
	FilterbankCompressorFactory *compressorFactory;
	AudioFrameReaderFactory *readerFactory;
	StimulusPlayer *player;
public:
	RefactoredModel(
		SpeechPerceptionTest *test,
		PrescriptionReader *prescriptionReader,
		BrirReader *brirReader,
		FilterbankCompressorFactory *compressorFactory,
		AudioFrameReaderFactory *readerFactory,
		StimulusPlayer *player
	) :
		prescriptionReader{ prescriptionReader },
		brirReader{ brirReader },
		test{ test },
		compressorFactory{ compressorFactory },
		readerFactory{ readerFactory },
		player{ player } {}

	void prepareNewTest(TestParameters p) override {
		prepareNewTest_(p);
		if (p.usingSpatialization)
			readBrir(p);
		if (p.usingHearingAidSimulation)
			readPrescriptions(p);
		testParameters = p;
	}

private:
	void prepareNewTest_(Model::TestParameters p) {
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

public:
	void playTrial(TrialParameters p) override {
		test->playNextTrial(player);
		auto reader = readerFactory->make(test->nextStimulus());
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

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }

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

	void readBrir(Model::TestParameters p) {
		try {
			brirReader->read(p.brirFilePath);
		}
		catch (const BrirReader::ReadFailure &) {
			throw TestInitializationFailure{ "Unable to read '" + p.brirFilePath + "'." };
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

class FailingBrirReader : public BrirReader {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	BinauralRoomImpulseResponse read(std::string) override {
		throw ReadFailure{ errorMessage };
	}
};

class SpeechPerceptionTestStub : public SpeechPerceptionTest {
	TestParameters testParameters_{};
	std::string nextStimulus_{};
	StimulusPlayer *player_{};
public:
	const TestParameters &testParameters() const {
		return testParameters_;
	}

	void prepareNewTest(TestParameters p) override {
		testParameters_ = std::move(p);
	}

	void playNextTrial(StimulusPlayer *p) override {
		player_ = p;
	}

	void setNextStimulus(std::string s) {
		nextStimulus_ = std::move(s);
	}

	std::string nextStimulus() override {
		return nextStimulus_;
	}

	const StimulusPlayer *player() const {
		return player_;
	}
};

class InitializationFailingSpeechPerceptionTest : public SpeechPerceptionTest {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void prepareNewTest(TestParameters) override {
		throw TestInitializationFailure{ errorMessage };
	}

	void playNextTrial(StimulusPlayer *) override {}
	std::string nextStimulus() override { return {}; }
};

#include "assert-utility.h"
#include "FilterbankCompressorSpy.h"
#include "AudioFrameReaderStub.h"
#include "StimulusPlayerStub.h"
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	RefactoredModel::TrialParameters trial{};
	PrescriptionReaderStub prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	FilterbankCompressorSpyFactory compressorFactory{};
	std::shared_ptr<AudioFrameReaderStub> reader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory readerFactory{reader};
	StimulusPlayerStub player{};
	RefactoredModel model{ 
		&test,
		&prescriptionReader, 
		&brirReader, 
		&compressorFactory, 
		&readerFactory,
		&player
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

TEST_F(RefactoredModelTests, playTrialPassesStimulusPlayerToSpeechPerceptionTest) {
	playTrial();
	EXPECT_EQ(&player, test.player());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioFilePathToFactory) {
	test.setNextStimulus("a");
	playTrial();
	assertEqual("a", readerFactory.filePath());
}

TEST_F(RefactoredModelTests, playTrialResetsReaderAfterComputingRms) {
	playTrial();
	EXPECT_TRUE(reader->readingLog().endsWith("reset "));
}

TEST_F(RefactoredModelTests, DISABLED_playTrialComputesCalibrationScalars) {
	//FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
	//fakeReader.setChannels(2);
	//setInMemoryReader(fakeReader);
	//preparation.level_dB_Spl = 7;
	//processorFactory.setFullScale_dB_Spl(8);
	playTrial();
//		auto desiredRms = std::pow(10.0, (7 - 8) / 20.0);
	/*assertEqual(
		{
			desiredRms / rms<float>({ 1, 3, 5 }),
			desiredRms / rms<float>({ 2, 4, 6 })
		},
		processorFactory.parameters().channelScalars,
		1e-6
	);*/
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

class RefactoredModelFailureTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	PrescriptionReaderStub defaultPrescriptionReader{};
	PrescriptionReader *prescriptionReader{&defaultPrescriptionReader};
	BrirReaderStub defaultBrirReader{};
	BrirReader *brirReader{&defaultBrirReader};
	SpeechPerceptionTestStub defaultTest{};
	SpeechPerceptionTest *test{&defaultTest};
	FilterbankCompressorSpyFactory defaultCompressorFactory{};
	FilterbankCompressorFactory *compressorFactory{&defaultCompressorFactory};
	AudioFrameReaderStubFactory defaultAudioReaderFactory{};
	AudioFrameReaderFactory *audioReaderFactory{&defaultAudioReaderFactory};
	StimulusPlayerStub defaultStimulusPlayer{};
	StimulusPlayer *stimulusPlayer{ &defaultStimulusPlayer };

	void assertPreparingNewTestThrowsTestInitializationFailure(std::string what) {
		RefactoredModel model{ 
			test,
			prescriptionReader, 
			brirReader, 
			compressorFactory, 
			audioReaderFactory,
			stimulusPlayer
		};
		try {
			model.prepareNewTest(testing);
			FAIL() << "Expected RefactoredModel::TestInitializationFailure.";
		}
		catch (const RefactoredModel::TestInitializationFailure & e) {
			assertEqual(std::move(what), e.what());
		}
	}
};

TEST_F(
	RefactoredModelFailureTests, 
	prepareNewTestThrowsTestInitializationFailureWhenPrescriptionReaderFails
) {
	FailingPrescriptionReader failing;
	failing.setErrorMessage("irrelevant");
	testing.usingHearingAidSimulation = true;
	testing.leftDslPrescriptionFilePath = "a";
	prescriptionReader = &failing;
	assertPreparingNewTestThrowsTestInitializationFailure("Unable to read 'a'.");
}

TEST_F(
	RefactoredModelFailureTests, 
	prepareNewTestThrowsTestInitializationFailureWhenTestFailsToInitialize
) {
	InitializationFailingSpeechPerceptionTest failing;
	failing.setErrorMessage("error.");
	test = &failing;
	assertPreparingNewTestThrowsTestInitializationFailure("error.");
}

TEST_F(
	RefactoredModelFailureTests, 
	prepareNewTestThrowsTestInitializationFailureWhenBrirReaderFails
) {
	FailingBrirReader failing;
	failing.setErrorMessage("irrelevant.");
	brirReader = &failing;
	testing.usingSpatialization = true;
	testing.brirFilePath = "a";
	assertPreparingNewTestThrowsTestInitializationFailure("Unable to read 'a'.");
}