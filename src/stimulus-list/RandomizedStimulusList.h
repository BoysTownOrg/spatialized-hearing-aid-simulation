#pragma once

#include "DirectoryReader.h"
#include "Randomizer.h"
#include "stimulus-list-exports.h"
#include <recognition-test/StimulusList.h>

class RandomizedStimulusList : public StimulusList {
    std::vector<std::string> files{};
    std::string current_{};
    std::string directory_{};
    DirectoryReader *reader;
    Randomizer *randomizer;
public:
    STIMULUS_LIST_API RandomizedStimulusList(DirectoryReader *reader, Randomizer *randomizer);
    void initialize(std::string directory) override;
    bool empty() override;
    std::string next() override;
};
