#include "FirFilter.h"
#include <gsl/gsl>
#include <algorithm>

FirFilter::FirFilter(std::vector<float> b) {
	if (b.size() == 0)
		throw InvalidCoefficients{};
	const auto M = b.size();
	int s{};
	auto dividend = M - 1;
	while (dividend /= 2)
		++s;
	N = 1 << (s + 1);
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

void FirFilter::process(float *x, int n) {
	for (int i = 0; i < n / L; ++i)
		filter(x + i * L, L);
	int samplesLeft = n % L;
	filter(x + n - samplesLeft, samplesLeft);
}

void FirFilter::filter(float *x, int samples) {
	std::fill(dftReal.begin(), dftReal.end(), 0.0f);
	for (int j = 0; j < samples; ++j)
		dftReal[j] = x[j];
	updateOverlap();
	for (int i = 0; i < samples; ++i)
		x[i] = overlap[i] / N;
	shiftOverlap(samples);
}

void FirFilter::updateOverlap() {
	fftwf_execute(fftPlan);
	for (int j = 0; j < N / 2 + 1; ++j)
		dftComplex[j] *= H[j];
	fftwf_execute(ifftPlan);
	for (int j = 0; j < N; ++j)
		overlap[j] += dftReal[j];
}

void FirFilter::shiftOverlap(int x) {
	for (int i = 0; i < N - x; ++i)
		overlap[i] = overlap[i + x];
	for (int i = N - x; i < N; ++i)
		overlap[i] = 0;
}
