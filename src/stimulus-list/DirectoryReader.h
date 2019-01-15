#pragma once

#include <common-includes/Interface.h>
#include <vector>
#include <string>

class DirectoryReader {
public:
    INTERFACE_OPERATIONS(DirectoryReader);
    virtual std::vector<std::string> filesIn(std::string directory) = 0;
};