#include "WindowsDirectoryReader.h"

// https://docs.microsoft.com/en-us/windows/desktop/fileio/listing-the-files-in-a-directory
WindowsDirectoryReader::WindowsDirectoryReader(std::string directory) :
	hFind{::FindFirstFileA((directory += "\\*").c_str(), &lpFindFileData)}
{
	if (failed())
		return;
	do {
		if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		else
			files_.push_back(lpFindFileData.cFileName);
	} while (::FindNextFileA(hFind, &lpFindFileData) != 0);
}

std::vector<std::string> WindowsDirectoryReader::files() {
	return files_;
}

bool WindowsDirectoryReader::failed() {
	return hFind == INVALID_HANDLE_VALUE;
}

// https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
std::string WindowsDirectoryReader::errorMessage() {
	LPSTR buffer{};
    auto size = ::FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, 
		::GetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPSTR)&buffer, 
		0, 
		nullptr
	);

    std::string message(buffer, size);
    ::LocalFree(buffer);

    return message;
}

WindowsDirectoryReader::~WindowsDirectoryReader() noexcept {
	FindClose(hFind);
}

std::shared_ptr<DirectoryReader> WindowsDirectoryReaderFactory::make(std::string directory)
{
	return std::make_shared<WindowsDirectoryReader>(std::move(directory));
}
