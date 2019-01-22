#pragma once

#include <recognition-test/Documenter.h>

class DocumenterStub : public Documenter {
	std::string filePath_{};
public:
	std::string filePath() const {
		return filePath_;
	}

	void initialize(std::string filePath) override {
		filePath_ = std::move(filePath);
	}
};