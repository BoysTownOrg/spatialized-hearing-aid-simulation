#pragma once

#include "LogString.h"
#include <recognition-test/Documenter.h>
#include <sstream>

class DocumenterStub : public Documenter {
	std::string filePath_{};
	std::string errorMessage_{};
	std::stringstream content_{};
	LogString log_{};
	bool failed_{};
public:
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

	void writeLine(std::string s) override {
		content_ << std::move(s) << '\n';
		log_ += LogString{ "writeLine " };
	}

	LogString log() const {
		return log_;
	}

	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	void fail() {
		failed_ = true;
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}
};