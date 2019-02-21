#pragma once

#include <presentation/Model.h>

class ModelStub : public Model {
	std::vector<std::string> audioDeviceDescriptions_{};
	TestParameters testParameters_{};
	TrialParameters trialParameters_{};
	CalibrationParameters calibrationParameters_{};
	SaveAudioParameters saveAudioParameters_{};
	double calibrationLevel_dB_Spl_{};
	bool testComplete_{};
	bool trialPlayed_{};
	bool calibrationStopped_{};
	bool calibrationPlayed_{};
	bool testPrepared_{};
public:
	auto saveAudioParameters() noexcept {
		return saveAudioParameters_;
	}

	auto calibrationParameters() noexcept {
		return calibrationParameters_;
	}

	void saveAudio(SaveAudioParameters *p) override {
		saveAudioParameters_ = *p;
	}

	auto calibrationLevel_dB_Spl() const noexcept {
		return calibrationLevel_dB_Spl_;
	}

	void stopCalibration() override {
		calibrationStopped_ = true;
	}

	auto calibrationStopped() const noexcept {
		return calibrationStopped_;
	}

	void playCalibration(CalibrationParameters *p) override {
		calibrationParameters_ = *p;
		calibrationPlayed_ = true;
	}

	auto calibrationPlayed() const noexcept {
		return calibrationPlayed_;
	}

	auto testParameters() const noexcept {
		return testParameters_;
	}

	void setAudioDeviceDescriptions(std::vector<std::string> d) {
		audioDeviceDescriptions_ = std::move(d);
	}

	void prepareNewTest(TestParameters *p) override {
		testParameters_ = *p;
		testPrepared_ = true;
	}

	auto testPrepared() const noexcept {
		return testPrepared_;
	}

	void setTestIncomplete() noexcept {
		testComplete_ = false;
	}

	auto trialPlayed() const noexcept {
		return trialPlayed_;
	}

	void playNextTrial(TrialParameters *p) override {
		trialParameters_ = *p;
		trialPlayed_ = true;
	}

	bool testComplete() override {
		return testComplete_;
	}

	auto trialParameters() const noexcept {
		return trialParameters_;
	}

	void setTestComplete() noexcept {
		testComplete_ = true;
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
	}
};

class FailingModel : public Model {
	std::string message{};
public:
	void setErrorMessage(std::string s) {
		message = std::move(s);
	}

	void prepareNewTest(TestParameters *) override {
		throw RequestFailure{ message };
	}

	void playNextTrial(TrialParameters *) override {
		throw RequestFailure{ message };
	}

	void playCalibration(CalibrationParameters *) override {
		throw RequestFailure{ message };
	}

	bool testComplete() override { return {}; }
	void stopCalibration() override {}
	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	void saveAudio(SaveAudioParameters *) override {}
};
