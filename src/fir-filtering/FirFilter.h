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
public:
	using coefficient_type = std::vector<float>;
	class InvalidCoefficients {};
	FIR_FILTERING_API explicit FirFilter(coefficient_type b);
	FIR_FILTERING_API ~FirFilter();
	FIR_FILTERING_API void process(gsl::span<float>) override;
	FIR_FILTERING_API index_type groupDelay() override;
private:
	std::vector<std::complex<float>> H{};
	std::vector<std::complex<float>> dftComplex{};
	std::vector<float> dftReal{};
	std::vector<float> overlap{};
	fftwf_plan fftPlan{};
	fftwf_plan ifftPlan{};
	long N{};
	coefficient_type::size_type L{};
	coefficient_type::size_type order;
	
	long nextPowerOfTwo(coefficient_type::size_type x);
	void filter(gsl::span<float>);
	void overlapAdd();
	void shiftOverlap(index_type x);
};
