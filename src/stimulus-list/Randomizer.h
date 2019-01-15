#pragma once

#include <common-includes/Interface.h>
#include <vector>
#include <string>

class Randomizer {
public:
    INTERFACE_OPERATIONS(Randomizer);
    using shuffle_iterator = std::vector<std::string>::iterator;
    virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
};