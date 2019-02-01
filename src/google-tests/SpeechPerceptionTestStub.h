#pragma once

#include <spatialized-hearing-aid-simulation/SpeechPerceptionTest.h>

class SpeechPerceptionTestStub : public SpeechPerceptionTest {
	TestParameters testParameters_{};
	std::string nextStimulus_{};
	bool advanceTrialCalled_{};
	bool prepareNewTestCalled_{};
	bool complete_{};
public:
	const auto &testParameters() const noexcept {
		return testParameters_;
	}

	void prepareNewTest(TestParameters p) override {
		testParameters_ = std::move(p);
		prepareNewTestCalled_ = true;
	}

	auto prepareNewTestCalled() const {
		return prepareNewTestCalled_;
	}

	void advanceTrial() override {
		advanceTrialCalled_ = true;
	}

	auto advanceTrialCalled() const noexcept {
		return advanceTrialCalled_;
	}

	void setNextStimulus(std::string s) {
		nextStimulus_ = std::move(s);
	}

	std::string nextStimulus() override {
		return nextStimulus_;
	}

	void setComplete() {
		complete_ = true;
	}

	bool testComplete() override {
		return complete_;
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

	void advanceTrial() override {}
	std::string nextStimulus() override { return {}; }
	bool testComplete() override { return {}; }
};