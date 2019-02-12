#pragma once

#include <common-includes/Interface.h>
#include <vector>
#include <string>
#include <memory>

class DirectoryReader {
public:
    INTERFACE_OPERATIONS(DirectoryReader);
    virtual std::vector<std::string> filesIn(std::string directory) = 0;
};

class DirectoryReaderFactory {
public:
    INTERFACE_OPERATIONS(DirectoryReaderFactory);
    virtual std::shared_ptr<DirectoryReader> make(std::string directory) = 0;
};