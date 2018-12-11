#pragma once

#ifdef FIR_FILTERING_EXPORTS
	#define FIR_FILTERING_API __declspec(dllexport)
#else
	#define FIR_FILTERING_API __declspec(dllimport)
#endif

#include <signal-processing/SignalProcessor.h>
#include <fftw3.h>
#include <vector>
#include <complex>

class FirFilter : public SignalProcessor {
	std::vector<std::complex<float>> H{};
	std::vector<std::complex<float>> dftComplex{};
	std::vector<float> dftReal{};
	std::vector<float> overlap{};
	fftwf_plan fftPlan{};
	fftwf_plan ifftPlan{};
	long N{};
	long L{};
public:
	class InvalidCoefficients {};
	FIR_FILTERING_API explicit FirFilter(std::vector<float> b);
	FIR_FILTERING_API ~FirFilter();
	FIR_FILTERING_API void process(float *, int) override;
private:
	void updateOverlap();
	void shiftOverlap(int x);
};
