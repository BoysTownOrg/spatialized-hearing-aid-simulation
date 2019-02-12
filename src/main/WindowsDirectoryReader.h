#pragma once

#include <stimulus-list/DirectoryReader.h>
#include <Windows.h>

class WindowsDirectoryReader : public DirectoryReader {
	std::vector<std::string> files_{};
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hFind;
public:
	WindowsDirectoryReader(std::string directory);
	~WindowsDirectoryReader() noexcept;
	WindowsDirectoryReader(WindowsDirectoryReader &&) = delete;
	WindowsDirectoryReader &operator=(WindowsDirectoryReader &&) = delete;
	WindowsDirectoryReader(const WindowsDirectoryReader &) = delete;
	WindowsDirectoryReader &operator=(const WindowsDirectoryReader &) = delete;
	std::vector<std::string> files() override;
};

class WindowsDirectoryReaderFactory : public DirectoryReaderFactory {
	std::shared_ptr<DirectoryReader> make(std::string directory) override;
};