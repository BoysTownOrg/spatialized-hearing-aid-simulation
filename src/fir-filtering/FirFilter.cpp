#include "FirFilter.h"
#include <gsl/gsl>
#include <algorithm>

FirFilter::FirFilter(vector_type b)
{
	if (b.size() == 0)
		throw InvalidCoefficients{};
	M = b.size();
	L = 16;
	N = L + M - 1;
	buffer.resize(N);
	fftIn = b;
	fftIn.resize(N);
	fftOut.resize(N / 2 + 1);
	ifftOut.resize(N);
	ifftIn.resize(N / 2 + 1);
	fftPlan = fftwf_plan_dft_r2c_1d(N, &fftIn[0], reinterpret_cast<fftwf_complex *>(&fftOut[0]), FFTW_ESTIMATE);
	ifftPlan = fftwf_plan_dft_c2r_1d(N, reinterpret_cast<fftwf_complex *>(&ifftIn[0]), &ifftOut[0], FFTW_ESTIMATE);
	fftwf_execute(fftPlan);
	H = fftOut;
}

void FirFilter::process(float *x, int n) {
	std::fill(fftIn.begin(), fftIn.end(), 0.0f);
	for (int j = 0; j < n; ++j)
		fftIn[j] = x[j];
	fftwf_execute(fftPlan);
	for (int j = 0; j < N / 2 + 1; ++j)
		ifftIn[j] = H[j] * fftOut[j];
	fftwf_execute(ifftPlan);
	for (int j = 0; j < N; ++j)
		buffer[j] += ifftOut[j];
	for (int i = 0; i < n; ++i)
		x[i] = buffer[i] / N;
	for (int i = 0; i < N - n; ++i)
		buffer[i] = buffer[i + n];
	for (int i = N - n; i < N; ++i)
		buffer[i] = 0;
}
