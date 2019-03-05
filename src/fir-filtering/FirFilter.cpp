#include "FirFilter.h"
#include <algorithm>

static auto fftw_plan_dft_r2c_1d_adapted(int n, double *in, FirFilter<double>::complex_type *out, unsigned int flags) noexcept {
	return fftw_plan_dft_r2c_1d(n, in, reinterpret_cast<fftw_complex *>(out), flags);
}

static auto fftw_plan_dft_r2c_1d_adapted(int n, float *in, FirFilter<float>::complex_type *out, unsigned int flags) noexcept {
	return fftwf_plan_dft_r2c_1d(n, in, reinterpret_cast<fftwf_complex *>(out), flags);
}

static auto fftw_plan_dft_c2r_1d_adapted(int n, typename FirFilter<double>::complex_type *in, double *out, unsigned int flags) noexcept {
	return fftw_plan_dft_c2r_1d(n, reinterpret_cast<fftw_complex *>(in), out, flags);
}

static auto fftw_plan_dft_c2r_1d_adapted(int n, typename FirFilter<float>::complex_type *in, float *out, unsigned int flags) noexcept {
	return fftwf_plan_dft_c2r_1d(n, reinterpret_cast<fftwf_complex *>(in), out, flags);
}

static auto fftw_execute_adapted(fftw_plan p) noexcept {
	return fftw_execute(p);
}

static auto fftw_execute_adapted(fftwf_plan p) noexcept {
	return fftwf_execute(p);
}

static auto fftw_destroy_plan_adapted(fftw_plan p) noexcept {
	return fftw_destroy_plan(p);
}

static auto fftw_destroy_plan_adapted(fftwf_plan p) noexcept {
	return fftwf_destroy_plan(p);
}

template<typename T>
static constexpr long nextPowerOfTwo(typename FirFilter<T>::coefficients_size_type x) noexcept {
	int power{};
	while (x /= 2)
		++power;
	return 1 << (power + 1);
}

template<typename T>
FirFilter<T>::FirFilter(coefficients_type b) :
	order{ b.size() - 1 }
{
	if (b.size() == 0)
		throw InvalidCoefficients{};

	N = nextPowerOfTwo<T>(order);
	L = N - order;
	overlap.resize(N);
	dftReal = std::move(b);
	dftReal.resize(N);
	dftComplex.resize(N/2 + 1);
	fftPlan = fftw_plan_dft_r2c_1d_adapted(
		N, 
		&dftReal.front(),
		&dftComplex.front(),
		FFTW_ESTIMATE
	);
	ifftPlan = fftw_plan_dft_c2r_1d_adapted(
		N, 
		&dftComplex.front(),
		&dftReal.front(),
		FFTW_ESTIMATE
	);
	fftw_execute_adapted(fftPlan);
	H = dftComplex;
}

template<typename T>
FirFilter<T>::~FirFilter() {
	fftw_destroy_plan_adapted(fftPlan);
	fftw_destroy_plan_adapted(ifftPlan);
}

template<typename T>
void FirFilter<T>::process(signal_type signal) {
	filterCompleteSegments(signal);
	filterRemaining(signal);
}

template<typename T>
void FirFilter<T>::filterCompleteSegments(signal_type signal) {
	for (coefficients_size_type i{ 0 }; i < signal.size() / L; ++i)
		filter(signal.subspan(i * L, L));
}

template<typename T>
void FirFilter<T>::filterRemaining(signal_type signal) {
	filter(signal.last(signal.size() % L));
}

template<typename T>
void FirFilter<T>::filter(signal_type signal) {
	std::fill(
		std::copy(signal.begin(), signal.end(), dftReal.begin()), 
		dftReal.end(), 
		sample_type{ 0 }
	);
	overlapAdd();
	for (index_type i{ 0 }; i < signal.size(); ++i)
		signal[i] = overlap.at(i) / N;
	shiftOverlap(signal.size());
}

template<typename T>
void FirFilter<T>::overlapAdd() {
	fftw_execute_adapted(fftPlan);
	std::transform(
		dftComplex.begin(), 
		dftComplex.end(), 
		H.begin(), 
		dftComplex.begin(), 
		std::multiplies{}
	);
	fftw_execute_adapted(ifftPlan);
	std::transform(
		overlap.begin(), 
		overlap.end(), 
		dftReal.begin(), 
		overlap.begin(), 
		std::plus{}
	);
}

template<typename T>
void FirFilter<T>::shiftOverlap(index_type n) {
	for (index_type i{ 0 }; i < N - n; ++i)
		overlap.at(i) = overlap.at(i + n);
	std::fill(overlap.end() - n, overlap.end(), sample_type{ 0 });
}

template<typename T>
auto FirFilter<T>::groupDelay() -> index_type {
	return order / 2;
}

template FirFilter<float>;
template FirFilter<double>;