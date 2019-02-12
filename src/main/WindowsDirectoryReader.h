#pragma once

#include <stimulus-list/DirectoryReader.h>

class WindowsDirectoryReader : public DirectoryReader {
	std::vector<std::string> files(std::string directory) override;
};