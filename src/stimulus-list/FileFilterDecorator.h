#pragma once
#include "RandomizedStimulusList.h"
#include "stimulus-list-exports.h"

class FileFilterDecorator : public DirectoryReader {
    std::string filter;
    DirectoryReader *reader;
public:
    STIMULUS_LIST_API FileFilterDecorator(DirectoryReader *reader, std::string filter);
    STIMULUS_LIST_API std::vector<std::string> filesIn(std::string directory) override;
private:
    std::vector<std::string> filtered(std::vector<std::string>);
};
