#pragma once

#include <recognition-test/Documenter.h>

class DocumenterStub : public Documenter {
	std::string filePath_{};
public:
	std::string filePath() const {
		return filePath_;
	}
};