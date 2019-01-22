#pragma once

#include <recognition-test/Documenter.h>

class DocumenterStub : public Documenter {
	std::string filePath_{};
	std::string content_{};
public:
	std::string filePath() const {
		return filePath_;
	}

	void initialize(std::string filePath) override {
		filePath_ = std::move(filePath);
	}

	std::string content() const {
		return content_;
	}

	void writeLine(std::string s) override {
		content_ += s + '\n';
	}
};