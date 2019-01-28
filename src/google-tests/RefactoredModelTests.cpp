#include <dsl-prescription/PrescriptionReader.h>
#include <presentation/Model.h>

class RefactoredModel : public Model {
	PrescriptionReader* prescriptionReader;
public:
	explicit RefactoredModel(PrescriptionReader* prescriptionReader) :
		prescriptionReader{ prescriptionReader } {}

	void initializeTest(TestParameters p) override {
		prescriptionReader->read(p.leftDslPrescriptionFilePath);
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