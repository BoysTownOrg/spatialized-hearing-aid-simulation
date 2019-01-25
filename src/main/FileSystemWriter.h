#pragma once

#include <spatialized-hearing-aid-simulation-utility/SpatializedHearingAidSimulationTestDocumenter.h>
#include <fstream>

class FileSystemWriter : public PersistentMemoryWriter {
	std::ofstream file;
public:
	void initialize(std::string filePath) override;
	void write(std::string s) override;
	bool failed() override;
	std::string errorMessage() override;
};