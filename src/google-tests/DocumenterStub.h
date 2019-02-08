#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/Documenter.h>
#include <sstream>

class DocumenterStub : public Documenter {
	Model::TestParameters *testParameters_{};
	TrialParameters trialParameters_{};
	LogString log_{};
	std::stringstream content_{};
	std::string filePath_{};
public:
	void documentTrialParameters(TrialParameters p) override {
		trialParameters_ = std::move(p);
		log_.insert("documentTrialParameters ");
	}

	auto documentedTrialParameters() {
		return trialParameters_;
	}

	void documentTestParameters(Model::TestParameters *p) override {
		testParameters_ = std::move(p);
		log_.insert("documentTestParameters ");
	}

	auto documentedTestParameters() {
		return testParameters_;
	}

	auto filePath() const {
		return filePath_;
	}

	void initialize(std::string filePath) override {
		filePath_ = std::move(filePath);
		log_.insert("initialize ");
	}

	auto content() const {
		return content_.str();
	}

	auto &log() const {
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

	void documentTestParameters(Model::TestParameters *) override {}
	void documentTrialParameters(TrialParameters) override {}
};