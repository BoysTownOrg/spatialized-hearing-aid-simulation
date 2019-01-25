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
	using sample_type = signal_type::element_type;
	using coefficient_type = std::vector<sample_type>;
	FIR_FILTERING_API explicit FirFilter(coefficient_type b);
	class InvalidCoefficients {};
	FIR_FILTERING_API ~FirFilter() override;
	FIR_FILTERING_API void process(signal_type) override;
	FIR_FILTERING_API index_type groupDelay() override;
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
	coefficient_type::size_type L{};
	coefficient_type::size_type order;
	
	long nextPowerOfTwo(coefficient_type::size_type x);
	void filterCompleteSegments(signal_type);
	void filterRemaining(signal_type);
	void filter(signal_type);
	void overlapAdd();
	void shiftOverlap(index_type x);
};
