#pragma once

#include <presentation/Model.h>
#include <presentation/Presenter.h>

class ModelStub : public Model {
	std::vector<std::string> audioDeviceDescriptions_{};
	TestParameters testParameters_{};
	TrialParameters trialParameters_{};
	bool testComplete_{};
	bool trialPlayed_{};
	bool calibrationPlayed_{};
public:
	GlobalTestParameters globalTestParameters{};

	bool calibrationPlayed() const {
		return calibrationPlayed_;
	}

	const TestParameters &testParameters() const {
		return testParameters_;
	}

	void setAudioDeviceDescriptions(std::vector<std::string> d) {
		audioDeviceDescriptions_ = std::move(d);
	}

	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
	}

	void initializeTest(TestParameters p) override {
		testParameters_ = std::move(p);
		globalTestParameters = *p.global;
	}

	void setTestIncomplete() {
		testComplete_ = false;
	}

	bool trialPlayed() const {
		return trialPlayed_;
	}

	void playTrial(TrialParameters p) override {
		trialParameters_ = std::move(p);
		trialPlayed_ = true;
	}

	bool testComplete() override {
		return testComplete_;
	}

	const TrialParameters &trialParameters() const {
		return trialParameters_;
	}

	void setTestComplete() {
		testComplete_ = true;
	}
};

class InitializationFailingModel : public Model {
	std::string message{};
public:
	void setErrorMessage(std::string s) {
		message = std::move(s);
	}

	void initializeTest(TestParameters) override {
		throw TestInitializationFailure{ message };
	}

	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	void playTrial(TrialParameters) override {}
	bool testComplete() override { return {}; }
};

class TrialFailingModel : public Model {
	std::string message{};
public:
	void setErrorMessage(std::string s) {
		message = std::move(s);
	}

	void playTrial(TrialParameters) override {
		throw TrialFailure{ message };
	}

	void initializeTest(TestParameters) override {}
	std::vector<std::string> audioDeviceDescriptions() override { return {}; }
	bool testComplete() override { return {}; }
};