#include "MersenneTwisterRandomizer.h"

void MersenneTwisterRandomizer::shuffle(shuffle_iterator begin, shuffle_iterator end) {
	std::shuffle(begin, end, engine);
}
