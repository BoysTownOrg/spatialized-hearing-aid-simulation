#pragma once

#include "ArgumentCollection.h"
#include <dsl-prescription/PrescriptionReader.h>
#include <map>

class PrescriptionReaderStub : public PrescriptionReader {
	ArgumentCollection<std::string> filePaths_{};
	std::map<std::string, Dsl> prescriptions_{};
public:
	void addPrescription(std::string filePath, Dsl dsl) {
		prescriptions_[filePath] = dsl;
	}

	Dsl read(std::string filePath) override {
		filePaths_.push_back(filePath);
		return prescriptions_[filePath];
	}

	ArgumentCollection<std::string> filePaths() const {
		return filePaths_;
	}
};

class FailingPrescriptionReader : public PrescriptionReader {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	Dsl read(std::string) override {
		throw ReadFailure{ errorMessage };
	}
};