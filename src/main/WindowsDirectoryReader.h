#pragma once

#include <stimulus-list/DirectoryReader.h>

class WindowsDirectoryReader : public DirectoryReader {
	std::vector<std::string> filesIn(std::string directory) override;
};