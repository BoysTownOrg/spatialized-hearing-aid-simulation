#pragma once

#include <fftw3.h>
#include <gsl/gsl>
#include <vector>
#include <complex>

#ifdef FIR_FILTERING_EXPORTS
	#define FIR_FILTERING_API __declspec(dllexport)
#else
	#define FIR_FILTERING_API __declspec(dllimport)
#endif

class FirFilter {
public:
	using signal_type = gsl::span<float>;
	using index_type = signal_type::index_type;
	using sample_type = signal_type::element_type;
	using coefficients_type = std::vector<sample_type>;
	FIR_FILTERING_API explicit FirFilter(coefficients_type b);
	class InvalidCoefficients {};
	FIR_FILTERING_API ~FirFilter();
	FirFilter(const FirFilter &) = delete;
	FirFilter &operator=(const FirFilter &) = delete;
	FirFilter(FirFilter&&) = delete;
    FirFilter& operator=(FirFilter&&) = delete;
	FIR_FILTERING_API void process(signal_type);
	FIR_FILTERING_API index_type groupDelay();
private:
	using complex_signal_type = std::vector<std::complex<sample_type>>;
	complex_signal_type H{};
	complex_signal_type dftComplex{};
	using real_signal_type = std::vector<sample_type>;
	real_signal_type dftReal{};
	real_signal_type overlap{};
	fftwf_plan fftPlan{};
	fftwf_plan ifftPlan{};
	long N{};
	coefficients_type::size_type L{};
	coefficients_type::size_type order;
	
	void filterCompleteSegments(signal_type);
	void filterRemaining(signal_type);
	void filter(signal_type);
	void overlapAdd();
	void shiftOverlap(index_type);
};
