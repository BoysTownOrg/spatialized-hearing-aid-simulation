#pragma once

#include "LogString.h"
#include <spatialized-hearing-aid-simulation/TestDocumenter.h>
#include <sstream>

class DocumenterStub : public TestDocumenter {
	TrialParameters trialParameters_{};
	LogString log_{};
	std::stringstream content_{};
	std::string filePath_{};
	const Model::Testing *testParameters_{};
public:
	void documentTrialParameters(TrialParameters p) override {
		trialParameters_ = std::move(p);
		log_.insert("documentTrialParameters ");
	}

	auto documentedTrialParameters() {
		return trialParameters_;
	}

	void documentTestParameters(const Model::Testing &p) override {
		testParameters_ = &p;
		log_.insert("documentTestParameters ");
	}

	auto documentedTestParameters() noexcept {
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

	auto &log() const noexcept {
		return log_;
	}
};

class InitializationFailingDocumenter : public TestDocumenter {
	std::string errorMessage_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	void initialize(std::string) override {
		throw InitializationFailure{ errorMessage_ };
	}

	void documentTestParameters(const Model::Testing &) override {}
	void documentTrialParameters(TrialParameters) override {}
};