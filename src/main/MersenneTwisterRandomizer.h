#pragma once

#include <stimulus-list/Randomizer.h>
#include <random>

class MersenneTwisterRandomizer : public Randomizer {
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
};