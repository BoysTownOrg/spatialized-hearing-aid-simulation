#pragma once

#include "DirectoryReader.h"
#include "Randomizer.h"
#include "stimulus-list-exports.h"
#include <spatialized-hearing-aid-simulation/StimulusList.h>

class RandomizedStimulusList : public StimulusList {
    std::vector<std::string> files{};
    std::string current_{};
    std::string directory_{};
	DirectoryReaderFactory *factory;
    Randomizer *randomizer;
public:
    STIMULUS_LIST_API RandomizedStimulusList(DirectoryReaderFactory *factory, Randomizer *randomizer);
    void initialize(std::string directory) override;
    bool empty() override;
    std::string next() override;
};
