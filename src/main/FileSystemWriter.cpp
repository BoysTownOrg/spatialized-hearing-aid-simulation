#include "FileSystemWriter.h"

void FileSystemWriter::initialize(std::string filePath) {
	file.open(std::move(filePath));
}

void FileSystemWriter::write(std::string s) {
	file << std::move(s);
}

bool FileSystemWriter::failed() {
	return file.fail();
}

std::string FileSystemWriter::errorMessage() {
	char buffer[256];
	strerror_s(buffer, sizeof buffer, errno);
	return buffer;
}

