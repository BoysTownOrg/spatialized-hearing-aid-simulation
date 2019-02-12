#pragma once

#include "DirectoryReader.h"
#include "stimulus-list-exports.h"

class FileFilterDecorator : public DirectoryReader {
    std::string filter;
	std::shared_ptr<DirectoryReader> reader;
public:
    STIMULUS_LIST_API FileFilterDecorator(std::shared_ptr<DirectoryReader> reader, std::string filter);
    STIMULUS_LIST_API std::vector<std::string> files() override;
private:
    std::vector<std::string> filtered(std::vector<std::string>);
};

class FileFilterDecoratorFactory : public DirectoryReaderFactory {
	std::string filter;
	DirectoryReaderFactory *decorated;
public:
	STIMULUS_LIST_API FileFilterDecoratorFactory(DirectoryReaderFactory *decorated, std::string filter);
	STIMULUS_LIST_API std::shared_ptr<DirectoryReader> make(std::string directory) override;
};
