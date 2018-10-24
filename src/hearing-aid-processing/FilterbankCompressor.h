#pragma once

#include <common-includes/Interface.h>

class FilterbankCompressor {
public:
	INTERFACE_OPERATIONS(FilterbankCompressor);
	using real = float;
	using complex = float;
	virtual void compressInput(
		real *input,
		real *output,
		int chunkSize) = 0;
	virtual void analyzeFilterbank(
		real *input,
		complex *output,
		int chunkSize) = 0;
	virtual void compressChannels(
		complex *input,
		complex *output,
		int chunkSize) = 0;
	virtual void synthesizeFilterbank(
		complex *input,
		real *output,
		int chunkSize) = 0;
	virtual void compressOutput(
		real *input,
		real *output,
		int chunkSize) = 0;
	virtual int chunkSize() const = 0;
	virtual int channels() const = 0;
};
