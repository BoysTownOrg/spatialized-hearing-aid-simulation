#pragma once

#include <spatialized-hearing-aid-simulation-utility/SpatializedHearingAidSimulationTestDocumenter.h>
#include <fstream>

class FileSystemWriter : public PersistentMemoryWriter {
	std::ofstream file;
public:
	void initialize(std::string filePath) override {
		file.open(std::move(filePath));
	}

	void write(std::string s) override {
		file << std::move(s);
	}

	bool failed() override {
		return file.fail();
	}

	std::string errorMessage() override {
		char buffer[256];
		strerror_s(buffer, sizeof buffer, errno);
		return buffer;
	}
};