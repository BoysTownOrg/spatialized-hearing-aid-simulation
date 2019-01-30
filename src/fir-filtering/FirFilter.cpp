#include "FirFilter.h"
#include <algorithm>

static constexpr long nextPowerOfTwo(FirFilter::coefficients_type::size_type x) noexcept {
	int power{};
	while (x /= 2)
		++power;
	return 1 << (power + 1);
}

FirFilter::FirFilter(coefficients_type b) :
	order{ b.size() - 1 }
{
	if (b.size() == 0)
		throw InvalidCoefficients{};
	N = nextPowerOfTwo(order);
	L = N - order;
	overlap.resize(N);
	dftReal = std::move(b);
	dftReal.resize(N);
	dftComplex.resize(N/2 + 1);
	const auto to_fftw = reinterpret_cast<fftwf_complex *>(&dftComplex.front());
	fftPlan = fftwf_plan_dft_r2c_1d(
		N, 
		&dftReal.front(),
		to_fftw,
		FFTW_ESTIMATE
	);
	ifftPlan = fftwf_plan_dft_c2r_1d(
		N, 
		to_fftw,
		&dftReal.front(),
		FFTW_ESTIMATE
	);
	fftwf_execute(fftPlan);
	H = dftComplex;
}

FirFilter::~FirFilter() {
	fftwf_destroy_plan(fftPlan);
	fftwf_destroy_plan(ifftPlan);
}

void FirFilter::process(signal_type signal) {
	filterCompleteSegments(signal);
	filterRemaining(signal);
}

void FirFilter::filterCompleteSegments(signal_type signal) {
	for (coefficients_type::size_type i = 0; i < signal.size() / L; ++i)
		filter(signal.subspan(i * L, L));
}

void FirFilter::filterRemaining(signal_type signal) {
	filter(signal.last(signal.size() % L));
}

void FirFilter::filter(signal_type signal) {
	std::fill(std::copy(signal.begin(), signal.end(), dftReal.begin()), dftReal.end(), sample_type{ 0 });
	overlapAdd();
	for (index_type i = 0; i < signal.size(); ++i)
		signal[i] = overlap.at(i) / N;
	shiftOverlap(signal.size());
}

void FirFilter::overlapAdd() {
	fftwf_execute(fftPlan);
	std::transform(
		dftComplex.begin(), 
		dftComplex.end(), 
		H.begin(), 
		dftComplex.begin(), 
		std::multiplies{}
	);
	fftwf_execute(ifftPlan);
	std::transform(
		overlap.begin(), 
		overlap.end(), 
		dftReal.begin(), 
		overlap.begin(), 
		std::plus{}
	);
}

void FirFilter::shiftOverlap(index_type n) {
	for (index_type i = 0; i < N - n; ++i)
		overlap.at(i) = overlap.at(i + n);
	std::fill(overlap.end() - n, overlap.end(), sample_type{ 0 });
}

auto FirFilter::groupDelay() -> index_type {
	return order / 2;
}
