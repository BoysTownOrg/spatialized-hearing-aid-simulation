#pragma once

#include "stimulus-list-exports.h"
#include <recognition-test/RecognitionTestModel.h>
#include <common-includes/Interface.h>

class DirectoryReader {
public:
    INTERFACE_OPERATIONS(DirectoryReader);
    virtual std::vector<std::string> filesIn(std::string directory) = 0;
};

class Randomizer {
public:
    INTERFACE_OPERATIONS(Randomizer);
    using shuffle_iterator = std::vector<std::string>::iterator;
    virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
};

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
