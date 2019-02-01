#pragma once

#include <stimulus-list/Randomizer.h>

class RandomizerStub : public Randomizer {
    std::vector<std::string> toShuffle_;
public:
    auto toShuffle() const {
        return toShuffle_;
    }
    
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        toShuffle_ = {begin, end};
    }
};