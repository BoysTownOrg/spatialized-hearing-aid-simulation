#include "FirFilter.h"
#include <gsl/gsl>
#include <algorithm>

FirFilter::FirFilter(vector_type b) {
	if (b.size() == 0)
		throw InvalidCoefficients{};
	M = b.size();
	L = 1024;
	N = L + M - 1;
	buffer.resize(N);
	fftIn = b;
	fftIn.resize(N);
	fftOut.resize(N / 2 + 1);
	fftPlan = fftwf_plan_dft_r2c_1d(
		N, 
		&fftIn[0], 
		reinterpret_cast<fftwf_complex *>(&fftOut[0]), 
		FFTW_ESTIMATE);
	ifftPlan = fftwf_plan_dft_c2r_1d(
		N, 
		reinterpret_cast<fftwf_complex *>(&fftOut[0]), 
		&fftIn[0], 
		FFTW_ESTIMATE);
	fftwf_execute(fftPlan);
	H = fftOut;
}

FirFilter::~FirFilter()
{
	fftwf_destroy_plan(fftPlan);
	fftwf_destroy_plan(ifftPlan);
}

void FirFilter::process(float *x, int n) {
	for (int i = 0; i < n / L; ++i) {
		std::fill(fftIn.begin(), fftIn.end(), 0.0f);
		for (int j = 0; j < L; ++j)
			fftIn[j] = x[j + i * L];
		fftwf_execute(fftPlan);
		for (int j = 0; j < N / 2 + 1; ++j)
			fftOut[j] *= H[j];
		fftwf_execute(ifftPlan);
		for (int j = 0; j < N; ++j)
			buffer[j] += fftIn[j];
		for (int j = 0; j < L; ++j)
			x[j + i * L] = buffer[j] / N;
		for (int j = 0; j < N - L; ++j)
			buffer[j] = buffer[j + L];
		for (int j = N - L; j < N; ++j)
			buffer[j] = 0;
	}
	int samplesLeft = n % L;
	std::fill(fftIn.begin(), fftIn.end(), 0.0f);
	for (int j = 0; j < samplesLeft; ++j)
		fftIn[j] = x[n - samplesLeft + j];
	fftwf_execute(fftPlan);
	for (int j = 0; j < N / 2 + 1; ++j)
		fftOut[j] *= H[j];
	fftwf_execute(ifftPlan);
	for (int j = 0; j < N; ++j)
		buffer[j] += fftIn[j];
	for (int i = 0; i < samplesLeft; ++i)
		x[n - samplesLeft + i] = buffer[i] / N;
	for (int i = 0; i < N - samplesLeft; ++i)
		buffer[i] = buffer[i + samplesLeft];
	for (int i = N - samplesLeft; i < N; ++i)
		buffer[i] = 0;
}
