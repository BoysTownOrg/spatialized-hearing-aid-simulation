#pragma once

#ifdef FIR_FILTERING_EXPORTS
	#define FIR_FILTERING_API __declspec(dllexport)
#else
	#define FIR_FILTERING_API __declspec(dllimport)
#endif

#include <vector>

class FirFilter {
	const std::vector<float> b;
	std::vector<float> delayLine;
public:
	class InvalidCoefficients {};
	FIR_FILTERING_API explicit FirFilter(std::vector<float> b);
	FIR_FILTERING_API void process(float *, int);

private:
	void filter(float *, int);
	void updateDelayLine(float *x, int n);
};
