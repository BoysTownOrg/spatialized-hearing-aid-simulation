#include "FirFilter.h"
#include <algorithm>

static long nextPowerOfTwo(unsigned int x) {
	int power{};
	while (x /= 2)
		++power;
	return 1 << (power + 1);
}

FirFilter::FirFilter(std::vector<float> b) {
	if (b.size() == 0)
		throw InvalidCoefficients{};
	const auto M = b.size();
	N = nextPowerOfTwo(M - 1);
	L = N - M + 1;
	overlap.resize(N);
	dftReal = b;
	dftReal.resize(N);
	dftComplex.resize(N / 2 + 1);
	const auto to_fftw = reinterpret_cast<fftwf_complex *>(&dftComplex[0]);
	fftPlan = fftwf_plan_dft_r2c_1d(
		N, 
		&dftReal[0],
		to_fftw,
		FFTW_ESTIMATE);
	ifftPlan = fftwf_plan_dft_c2r_1d(
		N, 
		to_fftw,
		&dftReal[0],
		FFTW_ESTIMATE);
	fftwf_execute(fftPlan);
	H = dftComplex;
}

FirFilter::~FirFilter() {
	fftwf_destroy_plan(fftPlan);
	fftwf_destroy_plan(ifftPlan);
}

void FirFilter::process(gsl::span<float> signal) {
	for (int i = 0; i < signal.size() / L; ++i)
		filter(signal.subspan(i * L, L));
	filter(signal.last(signal.size() % L));
}

void FirFilter::filter(gsl::span<float> signal) {
	std::fill(dftReal.begin(), dftReal.end(), 0.0f);
	for (int i = 0; i < signal.size(); ++i)
		dftReal[i] = signal[i];
	overlapAdd();
	for (int i = 0; i < signal.size(); ++i)
		signal[i] = overlap[i] / N;
	shiftOverlap(signal.size());
}

void FirFilter::overlapAdd() {
	fftwf_execute(fftPlan);
	for (int i = 0; i < N / 2 + 1; ++i)
		dftComplex[i] *= H[i];
	fftwf_execute(ifftPlan);
	for (int i = 0; i < N; ++i)
		overlap[i] += dftReal[i];
}

void FirFilter::shiftOverlap(int n) {
	for (int i = 0; i < N - n; ++i)
		overlap[i] = overlap[i + n];
	for (int i = N - n; i < N; ++i)
		overlap[i] = 0;
}
