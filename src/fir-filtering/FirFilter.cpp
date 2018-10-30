#include "FirFilter.h"
#include <gsl/gsl_util>
#include <algorithm>

FirFilter::FirFilter(std::vector<float> b) :
	b(std::move(b)),
	delayLine(this->b.size(), 0)
{
	if (this->b.size() == 0)
		throw InvalidCoefficients{};
}

void FirFilter::process(float *x, int n) {
	const auto size = gsl::narrow_cast<std::size_t>(n);
	for (std::size_t i = 0; i < size; ++i) {
		for (std::size_t j = 0; j + 1 < delayLine.size(); ++j)
			delayLine[j] = delayLine[j + 1];
		delayLine.back() = x[i];
		float accumulate = 0;
		for (std::size_t j = 0; j < b.size(); ++j)
			accumulate += b[j] * *(delayLine.end() - j - 1);
		x[i] = accumulate;
	}
}
