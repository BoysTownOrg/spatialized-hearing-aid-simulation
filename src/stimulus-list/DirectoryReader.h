#pragma once

#include <common-includes/Interface.h>
#include <vector>
#include <string>
#include <memory>

class DirectoryReader {
public:
    INTERFACE_OPERATIONS(DirectoryReader);
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
    virtual std::vector<std::string> files() = 0;
};

class DirectoryReaderFactory {
public:
    INTERFACE_OPERATIONS(DirectoryReaderFactory);
    virtual std::shared_ptr<DirectoryReader> make(std::string directory) = 0;
};