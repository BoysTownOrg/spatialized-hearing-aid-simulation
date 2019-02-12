#include "WindowsDirectoryReader.h"

// https://docs.microsoft.com/en-us/windows/desktop/fileio/listing-the-files-in-a-directory
WindowsDirectoryReader::WindowsDirectoryReader(std::string directory) :
	hFind{FindFirstFileA((directory += "\\*").c_str(), &lpFindFileData)}
{
	do {
		if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		else
			files_.push_back(lpFindFileData.cFileName);
	} while (FindNextFileA(hFind, &lpFindFileData) != 0);
}

std::vector<std::string> WindowsDirectoryReader::files() {
	return files_;
}

WindowsDirectoryReader::~WindowsDirectoryReader() noexcept {
	FindClose(hFind);
}

std::shared_ptr<DirectoryReader> WindowsDirectoryReaderFactory::make(std::string directory)
{
	return std::make_shared<WindowsDirectoryReader>(std::move(directory));
}
