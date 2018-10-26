#include "FirFilter.h"
#include <algorithm>

FirFilter::FirFilter(std::vector<float> b) :
	b(std::move(b))
{
	if (this->b.size() > 1) {
		delayLine.resize(this->b.size() - 1, 0);
		nextDelayLine.resize(this->b.size() - 1, 0);
	}
	else if (this->b.size() == 0)
		throw InvalidCoefficients{};
}

void FirFilter::process(float *x, int n) {
	updateNextDelayLine(x, n);
	filter(x, n);
	for (std::size_t i = 0; i < delayLine.size(); ++i)
		delayLine[i] = nextDelayLine[i];
}

void FirFilter::filter(float *x, int n) {
	const auto size = static_cast<std::size_t>(n);
	for (std::size_t i = size; i > 0; --i) {
		float accumulate = 0;
		for (
			std::size_t coefficientIndex = 0;
			coefficientIndex < b.size();
			++coefficientIndex)
		{
			const auto delayedInput = i > coefficientIndex
				? x[i - 1 - coefficientIndex]
				: *(delayLine.end() - coefficientIndex + i - 1);
			accumulate += b[coefficientIndex] * delayedInput;
		}
		x[i - 1] = accumulate;
	}
}


void FirFilter::updateNextDelayLine(float *x, int n) {
	const auto size = static_cast<std::size_t>(n);
	for (std::size_t i = 0; i + size < nextDelayLine.size(); ++i)
		nextDelayLine[i] = nextDelayLine[i + size];
	for (std::size_t i = 0; i < std::min(nextDelayLine.size(), size); ++i)
		*(nextDelayLine.end() - i - 1) = *(x + size - i - 1);
}
