#include "FirFilter.h"
#include <algorithm>

FirFilter::FirFilter(std::vector<float> b) :
	b(std::move(b))
{
	if (this->b.size() > 1)
		delayLine.resize(this->b.size() - 1, 0);
	else if (this->b.size() == 0)
		throw InvalidCoefficients{};
}

void FirFilter::process(float *x, int n) {
	filter(x, n);
	updateDelayLine(x, n);
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


void FirFilter::updateDelayLine(float *x, int n) {
	const auto size = static_cast<std::size_t>(n);
	for (std::size_t i = 0; i + size < delayLine.size(); ++i)
		delayLine[i] = delayLine[i + size];
	for (std::size_t i = 0; i < std::min(delayLine.size(), size); ++i)
		*(delayLine.end() - i - 1) = *(x - size - i - 1);
}
