#include "WindowsDirectoryReader.h"
#include <Windows.h>

class WindowsFileSearch {
	HANDLE hFind;
public:
	WindowsFileSearch(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) :
		hFind{FindFirstFileA(lpFileName, lpFindFileData)}
	{
	}

	bool FindNextFileA(LPWIN32_FIND_DATAA lpFindFileData) {
		return ::FindNextFileA(hFind, lpFindFileData);
	}

	~WindowsFileSearch() noexcept {
		FindClose(hFind);
	}

	WindowsFileSearch(WindowsFileSearch &&) = delete;
	WindowsFileSearch &operator=(WindowsFileSearch &&) = delete;
	WindowsFileSearch(const WindowsFileSearch &) = delete;
	WindowsFileSearch &operator=(const WindowsFileSearch &) = delete;
};

// https://docs.microsoft.com/en-us/windows/desktop/fileio/listing-the-files-in-a-directory
std::vector<std::string> WindowsDirectoryReader::filesIn(std::string directory) {
	directory += "\\*";
	WIN32_FIND_DATA ffd;
	WindowsFileSearch search{ directory.c_str(), &ffd };
	std::vector<std::string> files{};
	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		else
			files.push_back(ffd.cFileName);
	} while (search.FindNextFileA(&ffd) != 0);
	return files;
}
