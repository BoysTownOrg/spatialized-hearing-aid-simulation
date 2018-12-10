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
	using vector_type = std::vector<float>;
	using size_type = vector_type::size_type;
	// Order important for construction.
	std::vector<std::complex<float>> H{};
	std::vector<std::complex<float>> fftOut{};
	std::vector<float> fftIn{};
	std::vector<float> buffer{};
	fftwf_plan fftPlan{};
	fftwf_plan ifftPlan{};
	long N{};
	long L{};
	long M{};
public:
	class InvalidCoefficients {};
	FIR_FILTERING_API explicit FirFilter(vector_type b);
	FIR_FILTERING_API ~FirFilter();
	FIR_FILTERING_API void process(float *, int) override;
};
