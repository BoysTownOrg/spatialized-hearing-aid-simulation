#include "FirFilter.h"
#include <algorithm>

FirFilter::FirFilter(std::vector<float> b) :
	b(std::move(b)),
	delayLine(this->b.size(), 0)
{
	if (this->b.size() == 0)
		throw InvalidCoefficients{};
}

void FirFilter::process(float *x, int n) {
	const auto size = static_cast<std::size_t>(n);
	for (std::size_t i = 0; i < size; ++i) {
		for (std::size_t k = 0; k + 1 < delayLine.size(); ++k)
			delayLine[k] = delayLine[k + 1];
		delayLine.back() = x[i];
		float accumulate = 0;
		for (
			std::size_t coefficientIndex = 0;
			coefficientIndex < b.size();
			++coefficientIndex
			)
			accumulate += b[coefficientIndex] * *(delayLine.end() - coefficientIndex - 1);
		x[i] = accumulate;
	}
}
