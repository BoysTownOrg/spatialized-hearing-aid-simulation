#include "FileSystemWriter.h"
#include <string>

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
	return "Something unhelpful anyways...";
}

