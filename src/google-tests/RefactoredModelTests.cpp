#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
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

	struct TrialParameters {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void prepareNextTrial(TrialParameters) = 0;
	virtual void playTrial() = 0;
};

class RefactoredModel : public Model {
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	SpeechPerceptionTest *test;
public:
	RefactoredModel(
		SpeechPerceptionTest *test,
		AudioProcessor *,
		PrescriptionReader* prescriptionReader,
		BrirReader *brirReader,
		FilterbankCompressorFactory *
	) :
		prescriptionReader{ prescriptionReader },
		brirReader{ brirReader },
		test{ test } {}

	void prepareNewTest(TestParameters p) override {
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
		SpeechPerceptionTest::TrialParameters adapted;
		adapted.audioDevice = p.audioDevice;
		adapted.level_dB_Spl = p.level_dB_Spl;
		test->prepareNextTrial(adapted);
		test->playTrial();
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return {};
	}

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

class PrescriptionReaderStub : public PrescriptionReader {
	Dsl dsl_;
	ArgumentCollection<std::string> filePaths_{};
public:
	void setDsl(Dsl dsl) {
		dsl_ = std::move(dsl);
	}

	Dsl read(std::string filePath) override {
		filePaths_.push_back(std::move(filePath));
		return dsl_;
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
	TrialParameters trialParameters_{};
	std::string trialLog_{};
public:
	const TrialParameters &trialParameters() const {
		return trialParameters_;
	}

	const TestParameters &testParameters() const {
		return testParameters_;
	}

	void prepareNewTest(TestParameters p) override {
		testParameters_ = std::move(p);
	}

	void prepareNextTrial(TrialParameters p) override {
		trialParameters_ = std::move(p);
		trialLog_ += "prepareNextTrial ";
	}

	void playTrial() override {
		trialLog_ += "playTrial ";
	}

	std::string trialLog() const {
		return trialLog_;
	}
};

class AudioProcessorStub : public AudioProcessor {
	int sampleRate_{};
public:
	void setSampleRate(int fs) {
		sampleRate_ = fs;
	}
};

#include "assert-utility.h"
#include "FilterbankCompressorSpy.h"
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
	RefactoredModel model{ &test, &processor, &prescriptionReader, &brirReader, &compressorFactory };

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

TEST_F(RefactoredModelTests, playTrialPassesParametersToSpeechPerceptionTest) {
	trial.audioDevice = "a";
	trial.level_dB_Spl = 1.1;
	playTrial();
	assertEqual("a", test.trialParameters().audioDevice);
	EXPECT_EQ(1.1, test.trialParameters().level_dB_Spl);
}

TEST_F(RefactoredModelTests, playTrialPreparesSpeechPerceptionTestBeforePlaying) {
	playTrial();
	assertEqual("prepareNextTrial playTrial ", test.trialLog());
}

TEST_F(RefactoredModelTests, couldBeUgly) {
	PrescriptionReader::Dsl dsl;
	dsl.compressionRatios = { 1 };
	dsl.crossFrequenciesHz = { 2 };
	dsl.kneepointGains_dB = { 3 };
	dsl.kneepoints_dBSpl = { 4 };
	dsl.broadbandOutputLimitingThresholds_dBSpl = { 5 };
	dsl.channels = 6;
	prescriptionReader.setDsl(dsl);
	processor.setSampleRate(7);
	testing.usingHearingAidSimulation = true;
	testing.attack_ms = 8;
	testing.release_ms = 9;
	testing.chunkSize = 10;
	testing.windowSize = 11;
	prepareNewTest();
	playTrial();
	assertEqual({ 1 }, compressorFactory.parameters().compressionRatios);
}

class RefactoredModelWithFailingPrescriptionReaderTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	FailingPrescriptionReader prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	AudioProcessorStub processor{};
	FilterbankCompressorSpyFactory compressorFactory{};
	RefactoredModel model{ &test, &processor, &prescriptionReader, &brirReader, &compressorFactory };

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