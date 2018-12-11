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

FirFilter::~FirFilter()
{
	fftwf_destroy_plan(fftPlan);
	fftwf_destroy_plan(ifftPlan);
}

void FirFilter::process(float *x, int n) {
	for (int i = 0; i < n / L; ++i) {
		std::fill(dftReal.begin(), dftReal.end(), 0.0f);
		for (int j = 0; j < L; ++j)
			dftReal[j] = x[j + i * L];
		fftwf_execute(fftPlan);
		for (int j = 0; j < N / 2 + 1; ++j)
			dftComplex[j] *= H[j];
		fftwf_execute(ifftPlan);
		for (int j = 0; j < N; ++j)
			overlap[j] += dftReal[j];
		for (int j = 0; j < L; ++j)
			x[j + i * L] = overlap[j] / N;
		for (int j = 0; j < N - L; ++j)
			overlap[j] = overlap[j + L];
		for (int j = N - L; j < N; ++j)
			overlap[j] = 0;
	}
	int samplesLeft = n % L;
	std::fill(dftReal.begin(), dftReal.end(), 0.0f);
	for (int j = 0; j < samplesLeft; ++j)
		dftReal[j] = x[n - samplesLeft + j];
	fftwf_execute(fftPlan);
	for (int j = 0; j < N / 2 + 1; ++j)
		dftComplex[j] *= H[j];
	fftwf_execute(ifftPlan);
	for (int j = 0; j < N; ++j)
		overlap[j] += dftReal[j];
	for (int i = 0; i < samplesLeft; ++i)
		x[n - samplesLeft + i] = overlap[i] / N;
	for (int i = 0; i < N - samplesLeft; ++i)
		overlap[i] = overlap[i + samplesLeft];
	for (int i = N - samplesLeft; i < N; ++i)
		overlap[i] = 0;
}
