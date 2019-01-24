#pragma once

#include "LogString.h"
#include <recognition-test/Documenter.h>
#include <sstream>

class DocumenterStub : public Documenter {
	TestParameters testParameters_{};
	std::string filePath_{};
	std::stringstream content_{};
	LogString log_{};
public:
	void documentTestParameters(TestParameters p) override {
		testParameters_ = std::move(p);
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

	void write(std::string s) override {
		content_ << std::move(s);
		log_ += LogString{ "write " };
	}

	LogString log() const {
		return log_;
	}

	bool failed() override { return {}; }
	std::string errorMessage() override { return {}; }
};

class InitializationFailingDocumenter : public Documenter {
	std::string errorMessage_{};
	bool failed_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	void initialize(std::string) override {
		failed_ = true;
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}

	void write(std::string) override {}
	void documentTestParameters(TestParameters) override {}
};