#pragma once

#include "LogString.h"
#include <recognition-test/Documenter.h>

class DocumenterStub : public Documenter {
	TestParameters testParameters_{};
	TrialParameters trialParameters_{};
	std::string filePath_{};
	std::stringstream content_{};
	LogString log_{};
public:
	void documentTrialParameters(TrialParameters p) override {
		trialParameters_ = std::move(p);
		log_ += LogString{ "documentTrialParameters " };
	}

	TrialParameters documentedTrialParameters() {
		return trialParameters_;
	}

	void documentTestParameters(TestParameters p) override {
		testParameters_ = std::move(p);
		log_ += LogString{ "documentTestParameters " };
	}

	const TestParameters &documentedTestParameters() {
		return testParameters_;
	}

	std::string filePath() const {
		return filePath_;
	}

	void initialize(std::string filePath) override {
		filePath_ = std::move(filePath);
		log_ += LogString{ "initialize " };
	}

	std::string content() const {
		return content_.str();
	}

	LogString log() const {
		return log_;
	}
};

class InitializationFailingDocumenter : public Documenter {
	std::string errorMessage_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	void initialize(std::string) override {
		throw InitializationFailure{ errorMessage_ };
	}

	void documentTestParameters(TestParameters) override {}
	void documentTrialParameters(TrialParameters) override {}
};