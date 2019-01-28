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

class PrescriptionReaderStub : public PrescriptionReader {
	std::string filePath_{};
public:
	Dsl read(std::string filePath) override {
		filePath_ = std::move(filePath);
		return {};
	}

	std::string filePath() const {
		return filePath_;
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
	initializeTest();
	assertEqual("a", prescriptionReader.filePath());
}