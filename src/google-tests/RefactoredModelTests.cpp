#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <presentation/Model.h>

class SpeechPerceptionTest {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest);
	struct TestParameters {
		std::string audioDirectory;
		std::string testFilePath;
	};
};

class RefactoredModel : public Model {
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
public:
	RefactoredModel(
		PrescriptionReader* prescriptionReader,
		BrirReader *brirReader,
		SpeechPerceptionTest *
	) :
		prescriptionReader{ prescriptionReader },
		brirReader{ brirReader } {}

	void initializeTest(TestParameters p) override {
		if (p.usingSpatialization)
			brirReader->read(p.brirFilePath);
		if (p.usingHearingAidSimulation)
			readPrescriptions(p);
	}

	void playTrial(TrialParameters) override {
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
	ArgumentCollection<std::string> filePaths_{};
public:
	Dsl read(std::string filePath) override {
		filePaths_.push_back(std::move(filePath));
		return {};
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
public:
	const TestParameters &testParameters() const {
		return testParameters_;
	}
};

#include "assert-utility.h"
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	PrescriptionReaderStub prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	RefactoredModel model{ &prescriptionReader, &brirReader, &test };

	void initializeTest() {
		model.initializeTest(testing);
	}
};

TEST_F(RefactoredModelTests, initializeTestReadsPrescriptionsWhenUsingHearingAidSimulation) {
	testing.usingHearingAidSimulation = true;
	testing.leftDslPrescriptionFilePath = "a";
	testing.rightDslPrescriptionFilePath = "b";
	initializeTest();
	EXPECT_TRUE(prescriptionReader.filePaths().contains("a"));
	EXPECT_TRUE(prescriptionReader.filePaths().contains("b"));
}

TEST_F(RefactoredModelTests, initializeTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation) {
	testing.usingHearingAidSimulation = false;
	testing.leftDslPrescriptionFilePath = "a";
	testing.rightDslPrescriptionFilePath = "b";
	initializeTest();
	EXPECT_TRUE(prescriptionReader.filePaths().empty());
}

TEST_F(RefactoredModelTests, initializeTestReadsBrirWhenUsingSpatialization) {
	testing.usingSpatialization = true;
	testing.brirFilePath = "a";
	initializeTest();
	assertEqual("a", brirReader.filePath());
}

TEST_F(RefactoredModelTests, initializeTestDoesNotReadBrirWhenNotUsingSpatialization) {
	testing.usingSpatialization = false;
	testing.brirFilePath = "a";
	initializeTest();
	EXPECT_TRUE(brirReader.filePath().empty());
}

TEST_F(RefactoredModelTests, initializeTestPassesParametersToSpeechPerceptionTest) {
	testing.audioDirectory = "a";
	testing.testFilePath = "b";
	initializeTest();
	assertEqual("a", test.testParameters().audioDirectory);
	assertEqual("b", test.testParameters().testFilePath);
}

class RefactoredModelWithFailingPrescriptionReaderTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	FailingPrescriptionReader prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	RefactoredModel model{ &prescriptionReader, &brirReader, &test };

	void initializeTest() {
		model.initializeTest(testing);
	}
};

TEST_F(
	RefactoredModelWithFailingPrescriptionReaderTests, 
	initializeTestThrowsTestInitializationFailureWhenUsingHearingAidSimulation
) {
	prescriptionReader.setErrorMessage("irrelevant");
	try {
		testing.usingHearingAidSimulation = true;
		testing.leftDslPrescriptionFilePath = "a";
		initializeTest();
		FAIL() << "Expected RefactoredModel::TestInitializationFailure.";
	}
	catch (const RefactoredModel::TestInitializationFailure & e) {
		assertEqual("Unable to read 'a'.", e.what());
	}
}