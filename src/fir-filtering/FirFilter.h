#pragma once

#include <fftw3.h>
#include <gsl/gsl>
#include <vector>
#include <complex>

#ifdef _WIN32
    #ifdef FIR_FILTERING_EXPORTS
        #define FIR_FILTERING_API __declspec(dllexport)
    #else
        #define FIR_FILTERING_API __declspec(dllimport)
    #endif
#else
    #define FIR_FILTERING_API
#endif

template<typename T>
class FirFilter {
static_assert(
	std::is_same_v<T, float> || std::is_same_v<T, double>, 
	"FirFilter only supports float and double."
);

public:
	using signal_type = gsl::span<T>;
	using index_type = typename signal_type::index_type;
	using sample_type = typename signal_type::element_type;
	using coefficients_type = std::vector<sample_type>;
	using coefficients_size_type = typename coefficients_type::size_type;
	using complex_type = std::complex<sample_type>;
	using complex_signal_type = std::vector<complex_type>;
	using real_signal_type = std::vector<sample_type>;

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
	complex_signal_type H{};
	complex_signal_type dftComplex{};
	real_signal_type dftReal{};
	real_signal_type overlap{};
	using fftw_plan_type = typename std::conditional<
		std::is_same_v<sample_type, double>, 
		fftw_plan, 
		fftwf_plan
	>::type;
	fftw_plan_type fftPlan{};
	fftw_plan_type ifftPlan{};
	long N{};
	coefficients_size_type L{};
	coefficients_size_type order;
	
	void filterCompleteSegments(signal_type);
	void filterRemaining(signal_type);
	void filter(signal_type);
	void overlapAdd();
	void shiftOverlap(index_type);
};
