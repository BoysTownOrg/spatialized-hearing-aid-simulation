#pragma once

#ifdef FIR_FILTERING_EXPORTS
	#define FIR_FILTERING_API __declspec(dllexport)
#else
	#define FIR_FILTERING_API __declspec(dllimport)
#endif

#include <signal-processing/SignalProcessor.h>
#include <vector>

class FirFilter : public SignalProcessor {
	using vector = std::vector<float>;
	using size_type = vector::size_type;
	// Order important for construction.
	const vector b;
	vector delayLine;
public:
	class InvalidCoefficients {};
	FIR_FILTERING_API explicit FirFilter(vector b);
	FIR_FILTERING_API void process(float *, int) override;
};
