#include "FirFilter.h"
#include <algorithm>

FirFilter::FirFilter(coefficient_type b) :
	order{ b.size() - 1 }
{
	if (b.size() == 0)
		throw InvalidCoefficients{};
	const auto M = b.size();
	N = nextPowerOfTwo(M - 1);
	L = N - M + 1;
	overlap.resize(N);
	dftReal = std::move(b);
	dftReal.resize(N);
	dftComplex.resize(N / 2 + 1);
	const auto to_fftw = reinterpret_cast<fftwf_complex *>(&dftComplex[0]);
	fftPlan = fftwf_plan_dft_r2c_1d(
		N, 
		&dftReal[0],
		to_fftw,
		FFTW_ESTIMATE
	);
	ifftPlan = fftwf_plan_dft_c2r_1d(
		N, 
		to_fftw,
		&dftReal[0],
		FFTW_ESTIMATE
	);
	fftwf_execute(fftPlan);
	H = dftComplex;
}

long FirFilter::nextPowerOfTwo(coefficient_type::size_type x) {
	int power{};
	while (x /= 2)
		++power;
	return 1 << (power + 1);
}

FirFilter::~FirFilter() {
	fftwf_destroy_plan(fftPlan);
	fftwf_destroy_plan(ifftPlan);
}

void FirFilter::process(signal_type signal) {
	for (coefficient_type::size_type i = 0; i < signal.size() / L; ++i)
		filter(signal.subspan(i * L, L));
	filter(signal.last(signal.size() % L));
}

void FirFilter::filter(signal_type signal) {
	std::fill(dftReal.begin(), dftReal.end(), 0.0f);
	for (index_type i = 0; i < signal.size(); ++i)
		dftReal[i] = signal[i];
	overlapAdd();
	for (index_type i = 0; i < signal.size(); ++i)
		signal[i] = overlap[i] / N;
	shiftOverlap(signal.size());
}

void FirFilter::overlapAdd() {
	fftwf_execute(fftPlan);
	for (index_type i = 0; i < N / 2 + 1; ++i)
		dftComplex[i] *= H[i];
	fftwf_execute(ifftPlan);
	for (index_type i = 0; i < N; ++i)
		overlap[i] += dftReal[i];
}

void FirFilter::shiftOverlap(index_type n) {
	for (index_type i = 0; i < N - n; ++i)
		overlap[i] = overlap[i + n];
	for (index_type i = N - n; i < N; ++i)
		overlap[i] = 0;
}

auto FirFilter::groupDelay() -> index_type {
	return order / 2;
}
