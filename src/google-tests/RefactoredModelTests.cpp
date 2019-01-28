#include <dsl-prescription/PrescriptionReader.h>
#include <presentation/Model.h>

class RefactoredModel : public Model {
	PrescriptionReader* prescriptionReader;
public:
	explicit RefactoredModel(PrescriptionReader* prescriptionReader) :
		prescriptionReader{ prescriptionReader } {}

	void initializeTest(TestParameters p) override {
		try {
			prescriptionReader->read(p.leftDslPrescriptionFilePath);
			prescriptionReader->read(p.rightDslPrescriptionFilePath);
		}
		catch (const PrescriptionReader::ReadFailure &) {
			throw TestInitializationFailure{ "Unable to read '" + p.leftDslPrescriptionFilePath + "'." };
		}
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

#include "assert-utility.h"
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters test{};
	PrescriptionReaderStub prescriptionReader{};
	RefactoredModel model{&prescriptionReader};

	void initializeTest() {
		model.initializeTest(test);
	}
};

TEST_F(RefactoredModelTests, initializeTestReadsPrescriptionsWhenUsingHearingAidSimulation) {
	test.usingHearingAidSimulation = true;
	test.leftDslPrescriptionFilePath = "a";
	test.rightDslPrescriptionFilePath = "b";
	initializeTest();
	EXPECT_TRUE(prescriptionReader.filePaths().contains("a"));
	EXPECT_TRUE(prescriptionReader.filePaths().contains("b"));
}

TEST_F(RefactoredModelTests, initializeTestReadsBrirWhenUsingSpatialization) {
	test.usingSpatialization = true;
	test.brirFilePath = "a";
	initializeTest();
	assertEqual("a", brirReader.filePath());
}

class RefactoredModelWithFailingPrescriptionReaderTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters test{};
	FailingPrescriptionReader prescriptionReader{};
	RefactoredModel model{&prescriptionReader};

	void initializeTest() {
		model.initializeTest(test);
	}
};

TEST_F(
	RefactoredModelWithFailingPrescriptionReaderTests, 
	initializeTestThrowsTestInitializationFailureWhenUsingHearingAidSimulation
) {
	prescriptionReader.setErrorMessage("irrelevant");
	try {
		test.usingHearingAidSimulation = true;
		test.leftDslPrescriptionFilePath = "a";
		initializeTest();
		FAIL() << "Expected RefactoredModel::TestInitializationFailure.";
	}
	catch (const RefactoredModel::TestInitializationFailure & e) {
		assertEqual("Unable to read 'a'.", e.what());
	}
}