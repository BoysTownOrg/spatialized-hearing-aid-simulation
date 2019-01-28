#include <presentation/Model.h>

class RefactoredModel : public Model {
	void initializeTest(TestParameters) override {
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

#include "assert-utility.h"
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters test{};
	RefactoredModel model{};

	void initializeTest() {
		model.initializeTest(test);
	}
};

TEST_F(RefactoredModelTests, initializeTestReadsPrescriptionsWhenUsingHearingAidSimulation) {
	test.usingHearingAidSimulation = true;
	test.leftDslPrescriptionFilePath = "a";
	initializeTest();
	assertEqual("a", prescriptionReader->filePath());
}