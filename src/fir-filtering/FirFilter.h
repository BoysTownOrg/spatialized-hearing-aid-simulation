#pragma once

#ifdef FIR_FILTERING_EXPORTS
	#define FIR_FILTERING_API __declspec(dllexport)
#else
	#define FIR_FILTERING_API __declspec(dllimport)
#endif

#include <vector>

template<typename T>
class FirFilter {
	using vector = std::vector<T>;
	const vector b;
	vector delayLine;
public:
	class InvalidCoefficients {};
	FIR_FILTERING_API explicit FirFilter(vector b);
	FIR_FILTERING_API void process(T *, int);

private:
	void filter(T *, int);
	void updateDelayLine(T *x, int n);
};
