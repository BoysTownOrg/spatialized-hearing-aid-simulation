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

#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {

};

TEST_F(RefactoredModelTests, tbd) {
	FAIL();
}